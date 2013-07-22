
#ifndef __SINK_HPP_INCLUDED__
#define __SINK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>
#include <log4cxx/logger.h>

class ISinkWorker;

namespace zmq {
	class i_socket_t;
}

/*
	A Sink.
	
	Sinks receiver messages which are a tuple like this:
	
	<selector, msgid, data>
	
	selector == 1:
		first message, msgid contains the id of the first message
	
	selector == 2:
		work message, msgid contains the id of the message being processed and the data is the data.
		
	selector == 3:
		last message, msgid contains the id of the last message.
			
*/

class Sink {

private:

	log4cxx::LoggerPtr logger;
    zmq::i_socket_t *receiver;
    
public:
	Sink(log4cxx::LoggerPtr l, zmq::i_socket_t *r) : logger(l), receiver(r) {}
	
	bool process(ISinkWorker *worker);
	
};

#endif // __SINK_HPP_INCLUDED__

