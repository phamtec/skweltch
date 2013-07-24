
#ifndef __MSG_HPP_INCLUDED__
#define __MSG_HPP_INCLUDED__

#include <msgpack.hpp>
#include <zmq.hpp>

/**
	A Message
*/

template <typename Type>
class Msg {

protected:

	Type data;
	
public:
	Msg(const zmq::message_t &message) {
	
		msgpack::unpacked msg;
		msgpack::unpack(&msg, (const char *)message.data(), message.size());
		msgpack::object obj = msg.get();
		obj.convert(&data);
		
	}
	Msg(Type d) {
		data = d;
	}
	
	// set the data into a real message.
	void set(zmq::message_t *message) {

		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, data);
		message->rebuild(sbuf.size());
		memcpy(message->data(), sbuf.data(), sbuf.size());
	
	}

};

#endif // __MSG_HPP_INCLUDED__
