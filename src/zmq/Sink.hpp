
#ifndef __SINK_HPP_INCLUDED__
#define __SINK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class IMessager;
class JsonNode;

class Sink {

private:
	IMessager *msg;
	std::ostream *outfile;
		
public:
	Sink(IMessager *m, std::ostream *f) : msg(m), outfile(f) {}
	
	void service(JsonNode *root);
	
};

#endif // __SINK_HPP_INCLUDED__
