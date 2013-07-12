
#ifndef __JSONNAMEPREDICATE_HPP_INCLUDED__
#define __JSONNAMEPREDICATE_HPP_INCLUDED__

#include "JsonPredicate.hpp"

#include <string>

/**
	A predicate for searching for Json object by name
*/
class JsonNamePredicate : public JsonPredicate {

private:

	std::string name;
	
public:
	JsonNamePredicate(const std::string &n) : name(n) {}

	virtual bool match(const JsonObject &obj) const;

};

#endif // __JSONNAMEPREDICATE_HPP_INCLUDED__
