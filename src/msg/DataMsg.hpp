
#ifndef __DATAMSG_HPP_INCLUDED__
#define __DATAMSG_HPP_INCLUDED__

#include "Msg.hpp"
#include "IDataMsg.hpp"

/**
	Message that contains data.
*/

template <typename DataType>
class DataMsg : public Msg<std::pair<int, DataType> >, public IDataMsg {

public:
	DataMsg(const zmq::message_t &message) : Msg< std::pair<int, DataType> >(message) {}
	DataMsg(int id, DataType n) : Msg< std::pair<int, DataType> >(std::pair<int, DataType>(id, n)) {}
	
	// getters.
	virtual int getId() { 
		return this->data.first; 
	}
	DataType getPayload() { 
		return  this->data.second; 
	}
	
};

#endif // __DATAMSG_HPP_INCLUDED__
