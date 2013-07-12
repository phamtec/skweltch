
#ifndef __JSONARRAY_HPP_INCLUDED__
#define __JSONARRAY_HPP_INCLUDED__

#include <iostream>
#include "json_spirit_value.h"

class JsonObject;

/**
	A JSON Array.
*/
class JsonArray {

private:
	friend class JsonObject;
	
	json_spirit::Array _array;
	
public:
	JsonArray(const json_spirit::Array &array) : _array(array) {}
	JsonArray() {}

	typedef std::vector<json_spirit::Value>::iterator iterator;
	
	iterator begin() { return _array.begin(); }
	iterator end() { return _array.end(); }
	
	int getInt(iterator i, const std::string &name) const;
	std::string getString(iterator i, const std::string &name) const;
	JsonObject getValue(iterator i) const;
	std::string getChildAsString(iterator it, const std::string &key) const;
	bool empty() const { return _array.empty(); }	
	void add(JsonObject *o);

};

#endif // __JSONARRAY_HPP_INCLUDED__
