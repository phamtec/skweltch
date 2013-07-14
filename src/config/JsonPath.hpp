
#ifndef __JSONPATH_HPP_INCLUDED__
#define __JSONPATH_HPP_INCLUDED__

#include "JsonObject.hpp"

/**
	A JSON Path.
*/
class JsonPath {

public:

	JsonObject getPath(const JsonObject &obj, const std::string &path) const;

};

#endif // __JSONPATH_HPP_INCLUDED__
