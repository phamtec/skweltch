
#ifndef __JSONPREDICATE_HPP_INCLUDED__
#define __JSONPREDICATE_HPP_INCLUDED__

class JsonObject;

/**
	A predicate for searching for Json object
*/
class JsonPredicate {

public:

	virtual bool match(const JsonObject &obj) const = 0;

};

#endif // __JSONPREDICATE_HPP_INCLUDED__
