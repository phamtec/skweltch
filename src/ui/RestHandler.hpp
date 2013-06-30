
#ifndef __RESTHANDLER_HPP_INCLUDED__
#define __RESTHANDLER_HPP_INCLUDED__

#include <string>
#include <map>
#include <boost/function.hpp>

#include "reply.hpp"
#include "header.hpp"

class RestContext;

typedef boost::function<http::server::reply::status_type (
		RestContext *, 
		const std::string &args,
		std::vector<http::server::header> *headers,
		std::string *content
	)> handlerPointer;

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
