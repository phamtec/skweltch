#include "JsonConfig.hpp"
#include "Ports.hpp"
#include "Elapsed.hpp"
#include <zmq.hpp>
#include <czmq.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.singelapsed"));

int main (int argc, char *argv[])
{
 	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 4) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " pipes config name")
		return 1;
	}
	
	{
		stringstream outfn;
		outfn << "org.skweltch." << argv[3];
		logger = log4cxx::Logger::getLogger(outfn.str());
	}
		
 	JsonObject pipes;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}
 	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    
	try {
		int linger = -1;
		receiver.setsockopt(ZMQ_LINGER, &linger, sizeof linger);
 	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, e.what())
		return 1;
	}  
 
  	Ports ports(logger);
   	if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }

	Elapsed elapsed;
	int count = 0;
	int expect = 0;
    s_catch_signals ();
	while (!s_interrupted) {
	
		zmq::message_t message;
 		try {
       		receiver.recv(&message);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		}
		
		if (s_interrupted) {
			LOG4CXX_INFO(logger, "interrupt received, killing server")
			break;
		}

		msgpack::unpacked msg;
		msgpack::unpack(&msg, (const char *)message.data(), message.size());
		msgpack::object obj = msg.get();
		pair<int, int> sinkmsg;
		obj.convert(&sinkmsg);

		switch (sinkmsg.first) {
		case 1:
			expect = sinkmsg.second;
			count = 0;
 			LOG4CXX_INFO(logger, "Expecting: " << expect)
			elapsed.start();
			break;
			
		case 2:
			if (expect == 0) {
  				LOG4CXX_ERROR(logger, "Didn't get an expect message yet. discarding... " << obj)
   				return 1;
			}
			if (count == expect) {
			
  				LOG4CXX_INFO(logger, "Finished.")
    			
				 //  Calculate and report duration of batch
				int total_msec = elapsed.getTotal();

				// get results.
				JsonObject result;
				result.add("elapsed", total_msec);
				{
					ofstream results("results.json");
					result.write(true, &results);
				}
				
				count = 0;
			}
			else {
				count++;
			}
			break;
			
		default:
  			LOG4CXX_ERROR(logger, "Unknown message: " << sinkmsg.first)
   			return 1;
		}
	}
	   
   	LOG4CXX_INFO(logger, "finished.")

	return 0;
}
