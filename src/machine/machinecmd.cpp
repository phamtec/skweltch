
#include "MachineMsg.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

void packval(msgpack::sbuffer *sbuf, const char *s);
void packpair(msgpack::sbuffer *sbuf, const char *s1, const char *s2);
void packtuple(msgpack::sbuffer *sbuf, const char *s1, const char *s2, const char *s3);

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.machinecmd"));

int main (int argc, char *argv[])
{
	// Set up a simple configuration that logs on the console.
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

    try {
		po::positional_options_description pd;
		pd.add("cmd", 1);
		pd.add("jsonConfig", 2);

		po::options_description desc("options");
		desc.add_options()
			("help", "produce help message")
			("port", po::value<int>()->default_value(8000), "the port to connect to.")
			("cmd", po::value<string>(), "the message to send. start, vent or stop.")			
			("jsonConfig", po::value<string>(), "The filename for the config when starting.")			
		;

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
				  options(desc).positional(pd).run(), vm);
		po::notify(vm);

		// minimal args
        if (vm.count("help")) {
			LOG4CXX_ERROR(logger, desc)
            return 0;
        }
 
		//  Prepare our context and socket
		zmq::context_t context(1);
		zmq::socket_t sender(context, ZMQ_PUSH);
	
 		try {
 			stringstream ss;
 			ss << "tcp://localhost:" << vm["port"].as<int>();
			sender.connect(ss.str().c_str());
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't connect should be: tcp://localhost:port.")
			return 1;
		}

		MachineMsg msg;
		if (vm["cmd"].as<string>() == "start") {
			msg.startMsg(vm["jsonConfig"].as<string>());
		}
		else if (vm["cmd"].as<string>() == "vent") {
			msg.ventMsg();
		}
		else if (vm["cmd"].as<string>() == "stop") {
			msg.stopMsg();
		}
		else {
			LOG4CXX_ERROR(logger, "didn't understand command.")
			return 1;
		}
		
		zmq::message_t message(2);
		msg.set(&message);
 		try {
			sender.send(message);
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't send message.")
			return 1;
		}

	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
	
	return 0;
	
}
