
#ifndef __SINKMSG_HPP_INCLUDED__
#define __SINKMSG_HPP_INCLUDED__

#include "Msg.hpp"
#include <string>
#include <vector>

/**
	A message that you send/get to/from a Sink.
*/

class SinkMsg : public Msg<msgpack::type::tuple<int, int, std::vector<std::string> > > {

public:
	SinkMsg(const zmq::message_t &message) : Msg<msgpack::type::tuple<int, int, std::vector<std::string> > >(message) {}
	SinkMsg() : Msg<msgpack::type::tuple<int, int, std::vector<std::string> > >(
		msgpack::type::tuple<int, int, std::vector<std::string> >()) {}
	
	// getters.
	int getCode() const { return data.a0; }
	int getId() const { return data.a1; }
	std::vector<std::string> *getData() { return &data.a2; }
	
	// factories.
	void firstMsg(int id) 
		{ data.a0 = 1; data.a1 = id; }
	void lastMsg(int id)
		{ data.a0 = 3; data.a1 = id; }
	void dataMsg(int id, const std::vector<std::string> &d)
		{ data.a0 = 2; data.a1 = id; data.a2 = d; }
	
};

#endif // __SINKMSG_HPP_INCLUDED__

