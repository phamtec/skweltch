
#ifndef __XPATHPLITTER_HPP_INCLUDED__
#define __XPATHPLITTER_HPP_INCLUDED__

#include <string>
#include <iostream>

#include <log4cxx/logger.h>

class IChunk {

public:
	virtual bool chunk(const std::string &s) = 0;
};

class XPathSplitter {

private:
	log4cxx::LoggerPtr logger;
	std::istream *input;	
	
public:
	XPathSplitter(log4cxx::LoggerPtr l, std::istream *i) : logger(l), input(i) {}
	
	bool process(const std::string &xpath, IChunk *processor);
	
};

#endif // __XPATHPLITTER_HPP_INCLUDED__

