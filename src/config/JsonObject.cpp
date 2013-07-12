
#include "JsonObject.hpp"

#include "JsonArray.hpp"
#include "JsonPredicate.hpp"

#include "json_spirit.h"
#include "json_spirit_stream_reader.h"
#include "json_spirit_writer_template.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;
using namespace json_spirit;

bool JsonObject::isObject() const {

	return _value.type() == obj_type;
}

bool JsonObject::read(istream *istream) {

	try {
		read_stream_or_throw(*istream, _value);
	}
	catch (Error_position &p) {
		BOOST_LOG_TRIVIAL(error) << p.reason_ << " (" << p.line_ << ", " << p.column_ << ")";
		return false;
	}
	return true;
}

bool JsonObject::read(const string &s) {

	try {
		read_string_or_throw(s, _value);
	}
	catch (Error_position &p) {
		BOOST_LOG_TRIVIAL(error) << p.reason_ << " (" << p.line_ << ", " << p.column_ << ")";
		return false;
	}
	return true;

}

void  JsonObject::write(bool pp, ostream *ostream) const {

	if (_value.type() == null_type) {
		*ostream << "{}";
	}
	else {
		if (pp) {
			write_stream(_value, *ostream, pretty_print || remove_trailing_zeros);
		}
		else {
			write_stream(_value, *ostream, remove_trailing_zeros);
		}
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

void JsonObject::replace(const string &name, const JsonObject &o) {

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

void JsonObject::add(const std::string &name, const JsonArray &a) {

	if (_value.type() != obj_type) {
		_value = Object();
	}
	
	_value.get_obj().push_back(Pair(name, a._array)); 

}

string JsonObject::getKey(iterator it) {

	return it->name_;
	
}

JsonObject JsonObject::getValue(iterator it) {

	JsonObject o;
	o._value = it->value_;
	return o;
	
}

bool JsonObject::isValueArray(iterator it) {
	return it->value_.type() == array_type;
}

JsonArray JsonObject::getValueArray(iterator it) {
	return JsonArray(it->value_.get_array());
}

string JsonObject::getValueString(iterator it) {
	return it->value_.get_value<string>();
}

JsonObject JsonObject::findObj(const JsonPredicate &pred) {

	for (JsonObject::iterator i = begin(); i != end(); i++) {
		if (isValueArray(i)) {
			JsonArray a = getValueArray(i);
			for (JsonArray::iterator j = a.begin(); j != a.end(); j++) {
				JsonObject o = a.getValue(j);
				if (pred.match(o)) {
					return o;
				}
			}
		}
		else {
			JsonObject o = getValue(i);
			if (pred.match(o)) {
				return o;
			}
		}
	}
	return JsonObject();

}
