
#ifndef __RESTHANDLER_HPP_INCLUDED__
#define __RESTHANDLER_HPP_INCLUDED__

#include <string>
#include <map>
#include <boost/function.hpp>

#include "handlerType.hpp"

typedef boost::function<handlerType> handlerPointer;

/**
	A handler for REST
*/
class RestHandler {

private:

	std::map<std::string, handlerPointer> handlers;
	
public:
	RestHandler();
	
	void add(const std::string &path, handlerPointer func);
	bool isRest(const std::string &path);
	http::server::reply::status_type call(const std::string &path, std::vector<http::server::header> *headers, std::string *content);
};

#endif // __RESTHANDLER_HPP_INCLUDED__
