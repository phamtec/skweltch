
#include "JsonObject.hpp"

#include "JsonArray.hpp"

#include "json_spirit.h"
#include "json_spirit_stream_reader.h"
#include "json_spirit_writer_template.h"

using namespace std;
using namespace boost;
using namespace json_spirit;

bool JsonObject::read(istream *istream) {

	return read_stream(*istream, _value);

}

bool JsonObject::read(const string &s) {

	return read_string(s, _value);

}

void  JsonObject::write(bool pp, ostream *ostream) const {

	if (pp) {
    	write_stream(_value, *ostream, pretty_print || remove_trailing_zeros);
    }
    else {
    	write_stream(_value, *ostream, remove_trailing_zeros);
    }

}

string JsonObject::getChildAsString(const string &key) const {

	JsonObject o;
	o._value = get(_value.get_obj(), key);
	ostringstream ss;
	o.write(false, &ss);
	return ss.str();
	
}

JsonObject JsonObject::getChild(const string &key) const {

	JsonObject o;
	o._value = get(_value.get_obj(), key);
	return o;
	
}

Value JsonObject::get(const json_spirit::Object &o, const string &name) {

	for (vector<Pair>::const_iterator i=o.begin(); i != o.end(); i++) {
		if (name == i->name_) {
			return i->value_;
		}
	}
	return Value();

}

void JsonObject::setString(const string &key, const string &s) {

	set(&_value.get_obj(), key, Value(s));
	
}

void JsonObject::setInt(const string &key, int n) {

	set(&_value.get_obj(), key, Value(n));
	
}

void JsonObject::set(json_spirit::Object *o, const string &name, Value v) {

	for (vector<Pair>::iterator i=o->begin(); i != o->end(); i++) {
		if (name == i->name_) {
			i->value_ = v;
		}
	}

}

void JsonObject::replace(const std::string &name, const JsonObject &o) {

	set(&_value.get_obj(), name, o._value);

}


string JsonObject::getString(const string &key) const {

	return get(_value.get_obj(), key).get_value<string>();

}

int JsonObject::getInt(const string &key, int def) const {

	Value v = get(_value.get_obj(), key);
	if (v.type() == int_type) {
		return v.get_value<int>();
	}
	return def;

}

JsonArray JsonObject::getArray(const string &key) const {

	Value v = get(_value.get_obj(), key);
	if (v.type() == array_type) {
		return JsonArray(v.get_array());
	}

	return JsonArray(Array());

}

void JsonObject::add(const string &name, const string &s) {

	if (_value.type() != obj_type) {
		_value = Object();
	}
	
	_value.get_obj().push_back(Pair(name, Value(s))); 
	
}

void JsonObject::add(const string &name, int n) {

	if (_value.type() != obj_type) {
		_value = Object();
	}
	
	_value.get_obj().push_back(Pair(name, Value(n))); 
	
}

void JsonObject::add(const string &name, double n) {

	if (_value.type() != obj_type) {
		_value = Object();
	}
	
	_value.get_obj().push_back(Pair(name, Value(n))); 
	
}

void JsonObject::add(const string &name, const JsonObject &o) { 
	
	if (_value.type() != obj_type) {
		_value = Object();
	}
	
	_value.get_obj().push_back(Pair(name, o._value)); 

}

string JsonObject::getKey(iterator it) {

	return it->name_;
	
}

JsonObject JsonObject::getValue(iterator it) {

	JsonObject o;
	o._value = it->value_;
	return o;
	
}


