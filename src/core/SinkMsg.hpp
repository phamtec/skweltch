
#ifndef __SINKMSG_HPP_INCLUDED__
#define __SINKMSG_HPP_INCLUDED__

#include "Msg.hpp"

/**
	A message that you send/get to/from a Sink.
*/

class SinkMsg : public Msg<msgpack::type::tuple<int, int, int> > {

public:
	SinkMsg(const zmq::message_t &message) : Msg<msgpack::type::tuple<int, int, int> >(message) {}
	SinkMsg() : Msg<msgpack::type::tuple<int, int, int> >(msgpack::type::tuple<int, int, int>()) {}
	
	// getters.
	int getCode() { return data.a0; }
	int getId() { return data.a1; }
	int getData() { return data.a2; }
	
	// factories.
	void firstMsg(int id) 
		{ data.a0 = 1; data.a1 = id; data.a2 = 0; }
	void lastMsg(int id)
		{ data.a0 = 3; data.a1 = id; data.a2 = 0; }
	void dataMsg(int id, int d)
		{ data.a0 = 2; data.a1 = id; data.a2 = d; }
	
};

#endif // __SINKMSG_HPP_INCLUDED__

