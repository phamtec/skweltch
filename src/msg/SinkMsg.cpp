
#include "SinkMsg.hpp"

SinkMsg::SinkMsg(const zmq::message_t &message) {

	msgpack::unpacked msg;
	msgpack::unpack(&msg, (const char *)message.data(), message.size());
	msgpack::object obj = msg.get();
	obj.convert(&data);

}

void SinkMsg::set(zmq::message_t *message) {

	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, data);
	message->rebuild(sbuf.size());
	memcpy(message->data(), sbuf.data(), sbuf.size());
	
}

