
#ifndef __MSGTRACKER_HPP_INCLUDED__
#define __MSGTRACKER_HPP_INCLUDED__

#include <bitset>
#include <vector>
#include <log4cxx/logger.h>

/**
	A tracker for Messages
*/

class MsgTracker {

private:
	
	static const size_t SIZE = 1000;
    
	log4cxx::LoggerPtr logger;
	int first;
	int last;
	std::vector<std::bitset<SIZE> > bits;

	void grow(int n);
	
public:
	MsgTracker(log4cxx::LoggerPtr l) : logger(l), first(-1), last(-1) {}
	
	void setFirst(int f);
	void setLast(int l);
	
	void track(int n);
	bool complete();
	void reset();
    void dump();
    
};

#endif // __MSGTRACKER_HPP_INCLUDED__
