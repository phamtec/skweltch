
#ifndef __MACHINEMSG_HPP_INCLUDED__
#define __MACHINEMSG_HPP_INCLUDED__

#include "Msg.hpp"
#include <string>

/**
	A message that you send to a Machine.
*/

class MachineMsg : public Msg<std::pair<int, std::string> > {

public:
	MachineMsg(const zmq::message_t &message) : Msg<std::pair<int, std::string> >(message) {}
	MachineMsg() : Msg<std::pair<int, std::string> >(std::pair<int, std::string>()) {}
	
	// getters.
	int getCode() { return data.first; }
	std::string getData() { return data.second; }
	
	// factories.
	void startMsg(const std::string &config) 
		{ data.first = 1; data.second = config; }
	void ventMsg() 
		{ data.first = 2; }
	void stopMsg() 
		{ data.first = 3; }
		
};

#endif // __MACHINEMSG_HPP_INCLUDED__

