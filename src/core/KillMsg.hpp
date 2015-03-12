
#ifndef __KILLMSG_HPP_INCLUDED__
#define __KILLMSG_HPP_INCLUDED__

#include "Msg.hpp"

/**
	A message that is used to kill.
*/

class KillMsg : public Msg<int> {

public:
	KillMsg(const zmq::message_t &message) : Msg<int>(message) {}
	KillMsg() : Msg<int>(0) {}
	
	int value() {
		return data;
	}
};

#endif // __KILLMSG_HPP_INCLUDED__

