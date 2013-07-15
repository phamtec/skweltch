
#ifndef __JSONPATH_HPP_INCLUDED__
#define __JSONPATH_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>

/**
	A JSON Path.
*/
class JsonPath {

	json_spirit::Value *getPath(json_spirit::Value *obj, const std::string &path);

public:

	JsonObject getPath(const JsonObject &obj, const std::string &path) const;
	void setPathInt(JsonObject *obj, const std::string &path, const std::string &var, int n);

};

#endif // __JSONPATH_HPP_INCLUDED__
