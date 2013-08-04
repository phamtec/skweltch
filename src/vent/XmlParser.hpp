
#ifndef __XMLPARSER_HPP_INCLUDED__
#define __XMLPARSER_HPP_INCLUDED__

#include <string>
#include <iostream>
#include <log4cxx/logger.h>

class IXmlElement
{
public:
	virtual bool pi(const std::string &s) = 0;
	virtual bool startTag(const std::string &s) = 0;
	virtual bool attr(const std::string &s) = 0;
	virtual bool endAttr() = 0;
	virtual bool endTag(const std::string &s) = 0;
	virtual bool content(const std::string &s) = 0;
};

class XmlParser {

private:
	log4cxx::LoggerPtr logger;
	std::istream *input;	

public:
	XmlParser(log4cxx::LoggerPtr l, std::istream *i) : logger(l), input(i) {}
	
	bool process(IXmlElement *processor);
	
};

#endif // __XMLPARSER_HPP_INCLUDED__

