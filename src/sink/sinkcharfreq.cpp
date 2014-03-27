#include "Ports.hpp"
#include "Elapsed.hpp"
#include "Interrupt.hpp"
#include "Sink.hpp"
#include "ISinkWorker.hpp"
#include "Logging.hpp"
#include "Main.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.sinkcharfreq"));

class SWorker : public ISinkWorker {

private:
	int counts[26];

	void init();
	
public:
	SWorker() { init(); }
	
	virtual void first(int id) {}
	virtual void last(int id) {}
	
	virtual void process(int id, std::vector<std::string> *data);
	virtual void results(int total_ms);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

void SWorker::init() {

	// init for next time.
	for (size_t i=0; i<26; i++) {
		counts[i] = 0;
	}

}

void SWorker::process(int id, std::vector<std::string> *data) {

	LOG4CXX_DEBUG(logger, id << ", " << data)

	if ((*data)[0].length() > 26) {
		throw runtime_error("ony handle up to 26 letters.");
	}

	for (int i=0; i<26; i++) {

		char ss[2];
		ss[0] = (*data)[0][i];
		ss[1] = 0;
		int c;
		sscanf(ss, "%x", &c);		

		counts[i] += c;
	}

}

void SWorker::results(int total_ms) {

	JsonObject result;
	result.add("elapsed", total_ms);
	for (size_t i=0; i<26; i++) {
		char s[2];
		s[0] = 'a' + i;
		s[1] = 0;
		result.add(s, counts[i]);
	}
	
	// write the results.
	{
		ofstream results("temp-results.json");
		result.write(true, &results);
	}
	
	// and then rename for others to find it.
	filesystem::rename("temp-results.json", "results.json");

	init();
	
}

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    
 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }

	// turn on interrupts.
    s_catch_signals ();

	// and do the sink.
	Sink s(logger, &receiver);
	SWorker w;
	if (s.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
