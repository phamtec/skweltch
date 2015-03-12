
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "MachineRunner.hpp"
#include "Interrupt.hpp"
#include "Logging.hpp"
#include "TaskMonitor.hpp"
#include "Ports.hpp"
#include "KillMsg.hpp"

#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <msgpack.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.run"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);

    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "config to use.")
    ("control", po::value<int>()->default_value(7000), "The control port (bound)")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("jsonConfig")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
    
    s_catch_signals ();
    
	ifstream jsonfile(vm["jsonConfig"].as<string>().c_str());
	JsonObject r;
	if (!r.read(logger, &jsonfile)) {
        return 1;
	}

	TaskMonitor mon(logger, vm["control"].as<int>());

    zmq::context_t context(1);
    zmq::socket_t rsocket(context, ZMQ_PULL);
    rsocket.connect("tcp://localhost:6666");
    
    // create a control socket.
    zmq::socket_t csocket(context, ZMQ_PUB);
	if (!Ports::bind(&logger, &csocket, "*", vm["control"].as<int>(), "control")) {
        return 0;
	}

	// startup the machine.
	vector<int> pids;
	if (!mon.start(&r, &pids)) {
		throw new runtime_error("couldn't run the machine.");
	}
	
    // write the pids out so tools can kill if something goes wrong.
    {
        ofstream pidfile("task.pids");
        for (vector<int>::iterator i=pids.begin(); i != pids.end(); i++) {
            pidfile << *i << endl;
        }
    }
    
    // ok now we go for it.
	LOG4CXX_INFO(logger, "start run.")
	
	// do the vent
	{
		vector<int> vpids;
		if (!mon.doVent(&r, &vpids)) {
			return false;
		}
		// wait till vent is done.
		mon.waitFinish(vpids);
	}
		
	// and wait for the message to come.
	zmq::message_t message;
	try {
		rsocket.recv(&message);
	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, "recv failed." << e.what())
	}
	
	msgpack::unpacked msg;
	msgpack::unpack(&msg, (const char *)message.data(), message.size());
	msgpack::object obj = msg.get();
	
	LOG4CXX_INFO(logger, obj)

	// send a kill message on the control socket.
 	{
 		KillMsg msg;
		zmq::message_t message;
 		msg.set(&message);
		try {
			csocket.send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "control send failed." << e.what())
			return false;
		}
	}
		
	// wait till everything is gone.
	mon.waitFinish(pids);
	
	LOG4CXX_INFO(logger, "end run.")
	return 0;
	
}
