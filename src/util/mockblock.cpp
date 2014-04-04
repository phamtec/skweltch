
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "Interrupt.hpp"
#include "Logging.hpp"
#include "PipeBuilder.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.mockblock"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);
    pd.add("block", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "the machine config filename")
    ("block", po::value<string>(), "the name of a block to give info about.")
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
    
    string jsonConfig = vm["jsonConfig"].as<string>();
    if (!filesystem::exists(jsonConfig)) {
        LOG4CXX_ERROR(logger, jsonConfig << " does not exist");
        return 1;
    }
    string block = vm["block"].as<string>();
    
    try {
 	ifstream jsonfile(jsonConfig.c_str());
	JsonObject r;
	if (!r.read(logger, &jsonfile)) {
		return 1;
	}
        
        JsonArray blocks = r.getArray("blocks");
        for (JsonArray::iterator i = blocks.begin(); i != blocks.end(); i++) {
            JsonObject obj = blocks.getValue(i);
            if (obj.getString("name") == block) {

                //  Prepare our context and socket
                zmq::context_t context(1);
                
                JsonObject pipes = PipeBuilder(logger).collect(&r, obj);
                
                vector<boost::shared_ptr<zmq::socket_t> > sockets;
                vector<string> blocknames;
              
                for (JsonObject::iterator i = pipes.begin(); i != pipes.end(); i++) {
                    
                    JsonObject val = pipes.getValue(i);
                    string name = pipes.getKey(i);
                    
                    string mode = val.getString("mode");
                    
                    boost::shared_ptr<zmq::socket_t> socket(new zmq::socket_t(context, ZMQ_PULL));
                    
                    int port = val.getInt("port", -1);
                    cout << "Listing with " << mode;
                    
                    if (mode == "bind") {
                        stringstream ss;
                        ss << "tcp://" << val.getString("address") << ":" << port;
                        string s = ss.str();
                        socket->bind(s.c_str());
                        cout << " tcp://" << val.getString("address");
                    }
                    else if (mode == "connect") {
                        stringstream ss;
                        ss << "tcp://" << val.getString("node") << ":" << port;
                        string s = ss.str();
                        socket->connect(s.c_str());
                        cout << " tcp://" << val.getString("node");
                    }
                    else {
                        LOG4CXX_ERROR(logger, "bad mode");
                        return 1;
                    }
                    cout << ":" << port << endl;
                    
                    blocknames.push_back(name);
                    sockets.push_back(socket);
                }

                zmq::pollitem_t *items = new zmq::pollitem_t[sockets.size()];
                int n=0;
                for (vector<boost::shared_ptr<zmq::socket_t> >::iterator j = sockets.begin(); j != sockets.end(); j++, n++) {
                    items[n].socket = *(j->get());
                    items[n].events = ZMQ_POLLIN;
                }
                
                s_catch_signals ();
                while (true) {
                    try {
                        int rc = zmq::poll(items, sockets.size(), -1);
                        if (rc > 0) {
                            for (int i=0; i<sockets.size(); i++) {
                                if (items[i].revents & ZMQ_POLLIN) {
                                    zmq::message_t message;
                                    try {
                                        sockets[i]->recv(&message);
                                    }
                                    catch (zmq::error_t &e) {
                                        LOG4CXX_ERROR(logger, "recv failed." << e.what())
                                    }
                                    
                                    if (s_interrupted) {
                                        LOG4CXX_INFO(logger, "interrupt received, killing server")
                                        break;
                                    }
                                    
                                    msgpack::unpacked msg;
                                    msgpack::unpack(&msg, (const char *)message.data(), message.size());
                                    msgpack::object obj = msg.get();
                                    
                                    cout << blocknames[i] << ": " << obj << endl;
                                }
                            }
                        }
                        else if (rc == 0) { //timeout
                            LOG4CXX_DEBUG(logger, "timeout.");
                            ;
                        }
                        else {
                            LOG4CXX_ERROR(logger, "interrupted.");
                        }
                    }
                    catch( zmq::error_t& e ) {
                        LOG4CXX_ERROR(logger, "recv failed." << e.what());
                        break;
                    }
                }
                
                delete[] items;
                
                // no more.
                break;
            }
        }
 	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
    
    return 0;
}

