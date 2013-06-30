
#ifndef __REAP_HPP_INCLUDED__
#define __REAP_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class JsonNode;

class Reap {

private:
	std::ostream *outfile;
		
public:
	Reap(std::ostream *f) : outfile(f) {}
	
	void service(JsonNode *root, zmq::i_socket_t *receiver);
	
};

#endif // __REAP_HPP_INCLUDED__
