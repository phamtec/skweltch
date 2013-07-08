
#ifndef __JSONOBJECT_HPP_INCLUDED__
#define __JSONOBJECT_HPP_INCLUDED__

#include "json_spirit_value.h"
#include <iostream>
#include <string>

class JsonArray;

/**
	A JSON Object.
*/
class JsonObject {

private:
	friend class JsonArray;
	
	json_spirit::Value _value;
	
public:

	json_spirit::Value *getValue() { return &_value; }
	
	typedef std::vector<json_spirit::Pair>::iterator iterator;
	
	iterator begin() { return _value.get_obj().begin(); }
	iterator end() { return _value.get_obj().end(); }
	
	std::string getKey(iterator it);
	JsonObject getValue(iterator it);
	
	// reading and writing.
	bool read(std::istream *istream);
	bool read(const std::string &s);
	void write(bool pp, std::ostream *ostream) const;
	
	// get different things from the object.
	std::string getString(const std::string &key) const;
	void setString(const std::string &key, const std::string &s);
	
	int getInt(const std::string &key, int def) const;
	void setInt(const std::string &key, int n);
	
	JsonArray getArray(const std::string &key) const;
	JsonObject getChild(const std::string &key) const;
	std::string getChildAsString(const std::string &key) const;
	bool empty() const { return _value.type() == json_spirit::null_type; }
	
	// add to the object.
	void add(const std::string &name, const std::string &s);
	void add(const std::string &name, int n);
	void add(const std::string &name, double n);
	void add(const std::string &name, const JsonObject &o);
	void replace(const std::string &name, const JsonObject &o);

	// utility class.
	static json_spirit::Value get(const json_spirit::Object &o, const std::string &name);
	static void set(json_spirit::Object *o, const std::string &name, json_spirit::Value v);

};

#endif // __JSONOBJECT_HPP_INCLUDED__
