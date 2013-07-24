
#ifndef __STRINGMSG_HPP_INCLUDED__
#define __STRINGMSG_HPP_INCLUDED__

#include "DataMsg.hpp"
#include <string>

/**
	A message that contains a string.
*/

class StringMsg : public DataMsg<std::string> {

public:
	StringMsg(const zmq::message_t &message): DataMsg<std::string>(message) {}
	StringMsg(int id, const std::string &s) : DataMsg<std::string>(id, s) {}
	
};

#endif // __STRINGMSG_HPP_INCLUDED__
