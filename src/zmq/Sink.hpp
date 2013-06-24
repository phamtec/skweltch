
#ifndef __SINK_HPP_INCLUDED__
#define __SINK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class IMessager;
class JsonNode;

class Sink {

private:
	IMessager *msg;
	std::ofstream *outfile;
		
public:
	Sink(IMessager *m, std::ofstream *f) : msg(m), outfile(f) {}
	
	void service(JsonNode *root);
	
};

#endif // __SINK_HPP_INCLUDED__
