
#ifndef __JSONPATH_HPP_INCLUDED__
#define __JSONPATH_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>
#include <log4cxx/logger.h>

/**
	A JSON Path.
*/
class JsonPath {

private:
	json_spirit::Value *getPath(json_spirit::Value *obj, const std::string &path);
	json_spirit::Value *getChildWithCondition(json_spirit::Object *obj, const std::string &condition);
	void dump(json_spirit::Value *v);
	
	log4cxx::LoggerPtr logger;

public:
	JsonPath(log4cxx::LoggerPtr l) : logger(l) {}

	JsonObject getPath(const JsonObject &obj, const std::string &path) const;
	void setPathInt(JsonObject *obj, const std::string &path, const std::string &var, int n);

};

#endif // __JSONPATH_HPP_INCLUDED__
