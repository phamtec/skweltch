
#ifndef __SINK_HPP_INCLUDED__
#define __SINK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class JsonNode;

class Sink {

private:
	std::ostream *outfile;
		
public:
	Sink(std::ostream *f) : outfile(f) {}
	
	void service(JsonNode *root, zmq::i_socket_t *receiver);
	
};

#endif // __SINK_HPP_INCLUDED__
