
#ifndef __DATAMSG_HPP_INCLUDED__
#define __DATAMSG_HPP_INCLUDED__

#include "Msg.hpp"
#include "IDataMsg.hpp"

/**
	Message that contains data.
*/

template <typename DataType>
class DataMsg : public Msg<msgpack::type::tuple<int, clock_t, DataType> >, public IDataMsg {

public:
	DataMsg(const zmq::message_t &message) : Msg<msgpack::type::tuple<int, clock_t, DataType> >(message) {}
	DataMsg(int id, clock_t t, DataType n) : Msg<msgpack::type::tuple<int, clock_t, DataType> >(
		msgpack::type::tuple<int, clock_t, DataType>(id, t, n)) {}
	
	// getters.
	virtual int getId() { 
		return this->data.a0; 
	}
	virtual clock_t getTime() {
		return this->data.a1;
	}
	DataType getPayload() { 
		return  this->data.a2; 
	}
	
};

#endif // __DATAMSG_HPP_INCLUDED__
