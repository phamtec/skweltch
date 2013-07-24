
#ifndef __INTMSG_HPP_INCLUDED__
#define __INTMSG_HPP_INCLUDED__

#include "DataMsg.hpp"

/**
	A message that contains an int.
*/

class IntMsg : public DataMsg<int> {

public:
	IntMsg(const zmq::message_t &message) : DataMsg<int>(message) {}
	IntMsg(int id, int n) : DataMsg<int>(id, n) {}
	
};

#endif // __INTMSG_HPP_INCLUDED__

