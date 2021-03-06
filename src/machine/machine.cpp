
#include "TaskMonitor.hpp"
#include "Interrupt.hpp"
#include "MachineMsg.hpp"
#include "JsonObject.hpp"
#include "Logging.hpp"
#include "Ports.hpp"
#include "KillMsg.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <mongo/client/dbclient.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.machine"));

bool stop(TaskMonitor *mon, JsonObject *r, vector<int> *pids, zmq::socket_t *csocket);

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::options_description desc("options");
    desc.add_options()
        ("help", "produce help message")
        ("port", po::value<int>()->default_value(8000), "the port to bind to.")
		("results", po::value<string>()->default_value("tcp://localhost:6666"), "The results port.")
        ("control", po::value<int>()->default_value(7000), "the control port to bind to.")
        ("dbHost", po::value<string>()->default_value("localhost"), "the control port to bind to.")
        ("machineTable", po::value<string>(), "the database table where we can find machines.")
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).run(), vm);
    po::notify(vm);

	if (vm.count("help") || !vm.count("machineTable")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
	}
	
	mongo::client::initialize();
	mongo::DBClientConnection c;
	c.connect(vm["dbHost"].as<string>());
	
    try {
        // help with the ZMQ version.
        int major, minor, patch;
        zmq_version(&major, &minor, &patch);
        LOG4CXX_INFO(logger, "using ZMQ " << major << "." << minor << "." << patch)

     	//  Prepare our context and socket
    	zmq::context_t context(1);
    	zmq::socket_t receiver(context, ZMQ_PULL);
		if (!Ports::bind(&logger, &receiver, "*", vm["port"].as<int>(), "command")) {
        	return 0;
		}    	
		zmq::socket_t csocket(context, ZMQ_PUB);
		if (!Ports::bind(&logger, &csocket, "*", vm["control"].as<int>(), "control")) {
        	return 0;
		}

		enum {
			IDLE = 0,
			RUNNING = 1
		} state = IDLE;
	
		JsonObject r;	
		vector<int> pids;
		TaskMonitor mon(logger, vm["control"].as<int>());
			
		s_catch_signals ();
		while (1) {
	
			zmq::message_t message;
			try {
				receiver.recv(&message);
			}
			catch (zmq::error_t &e) {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		
			if (s_interrupted) {
				LOG4CXX_INFO(logger, "interrupt received, killing server")
				break;
			}
		
			MachineMsg msg(message);
		
			switch (msg.getCode()) {
			case 1:
			case 2:
				{
					if (state == RUNNING) {
						if (!stop(&mon, &r, &pids, &csocket)) {
							return 1;
						}
						pids.clear();
					}
                    if (msg.getCode() == 1) {
                    	string id = msg.getData();
                        mongo::BSONObj fields = BSON("_id" << 0);
						auto_ptr<mongo::DBClientCursor> cursor = c.query(vm["machineTable"].as<string>(),
							MONGO_QUERY("_id" << mongo::OID(id)), 0, 0, &fields);
						if (!cursor->more()) {
							LOG4CXX_ERROR(logger, "could not find machine in DB.")
						}
						else {
							stringstream ss;
							ss << cursor->next().jsonString();
							if (!r.read(logger, &ss)) {
								continue;
							}
						}
                    }
                    else {
						string config = msg.getData();
						ifstream jsonfile(config.c_str());
						if (!r.read(logger, &jsonfile)) {
							continue;
						}
					}
					if (!mon.start(&r, &pids)) {
						return 1;
					}
					state = RUNNING;
				}
				break;
				
			case 3:
				if (state != RUNNING) {
					LOG4CXX_ERROR(logger, "not running.")
				}
				else {
					vector<int> pids;
					if (!mon.doVent(&r, &pids)) {
						return 1;
					}
					mon.waitFinish(pids);
				}
				break;
				
			case 4:
				if (state != RUNNING) {
					LOG4CXX_ERROR(logger, "not running.")
				}
				else {
					if (!stop(&mon, &r, &pids, &csocket)) {
						return 1;
					}
					pids.clear();
					state = IDLE;
				}
				break;
                    
            case 5:
                if (state == RUNNING) {
                    if (!stop(&mon, &r, &pids, &csocket)) {
                        return 1;
                    }
                }
                LOG4CXX_INFO(logger, "quitting.");
                return 0;
                    
			}
		}
	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
    
    return 0;
	
}

bool stop(TaskMonitor *mon, JsonObject *r, vector<int> *pids, zmq::socket_t *csocket) {

	// send a kill message on the control socket.
	KillMsg msg;
	zmq::message_t message;
	msg.set(&message);
	try {
		csocket->send(message);
	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, "control send failed." << e.what())
		return false;
	}

	// wait till everything is gone.
	mon->waitFinish(*pids);
	
    pids->clear();
    
	return true;
}
