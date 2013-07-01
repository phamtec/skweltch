
#ifndef __HANDLERTYPE_HPP_INCLUDED__
#define __HANDLERTYPE_HPP_INCLUDED__

#include <string>
#include <map>
#include <boost/function.hpp>

#include "reply.hpp"
#include "header.hpp"

class RestContext;

typedef http::server::reply::status_type (handlerType)(
		RestContext *, 
		const std::string &args,
		std::vector<http::server::header> *headers,
		std::string *content
	);

#endif // __HANDLERTYPE_HPP_INCLUDED__

