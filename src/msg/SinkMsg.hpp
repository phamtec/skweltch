
#ifndef __SINKMSG_HPP_INCLUDED__
#define __SINKMSG_HPP_INCLUDED__

#include <msgpack.hpp>
#include <zmq.hpp>

/**
	A message that you send/get from/to a Sink.
*/

class SinkMsg {

private:

	msgpack::type::tuple<int, int, int> data;
	
public:
	SinkMsg(const zmq::message_t &message);
	SinkMsg() {}
	
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
		
	// set the data into a real message.
	void set(zmq::message_t *message);
	
};

#endif // __SINKMSG_HPP_INCLUDED__

