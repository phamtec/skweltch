
#include "JsonPath.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"

#include "json_spirit.h"
#include "json_spirit_writer_template.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
 
using namespace std;
using namespace boost;
using namespace json_spirit;

string nextToken(string *remain) {

	size_t pos = remain->find_first_of(".[");
	if (pos == string::npos) {
		string s = *remain;
		*remain = "";
		return s;
	}
	else {
		string s = remain->substr(0, pos);
		*remain = remain->substr(pos);
		return s;
	}
}

string getCondition(string *remain) {

	size_t pos = remain->find_first_of("]");
	if (pos == string::npos) {
		throw runtime_error("no end for condition");
	}
	else {
		string cond = remain->substr(0, pos);
		*remain = remain->substr(pos+1);
		return cond;
	}
}

Value *get(Object *obj, const string &name) {

	for (vector<Pair>::iterator i=obj->begin(); i != obj->end(); i++) {
		if (name == i->name_) {
			return &i->value_;
		}
	}
	return NULL;

}

bool valueMatches(Value *obj, const string &key, const string &value) {

	Value *v = get(&obj->get_obj(), key);
	if (v == NULL) {
		return false;
	}
	if (v->type() == int_type) {
   		int val = lexical_cast<int>(value);
		return v->get_value<int>() == val;
	}
	else if (v->type() == str_type) {
		return v->get_value<string>() == value;
	}
	return false;

}

bool childMatches(Value *obj, const string &condition) {

	// nvp
	vector<string> s;
    split(s, condition, is_any_of("="));
    
    // split possible quotes off key.
    string key = (s[0][0] == '"') ? s[0].substr(1, s[0].length()-2) : s[0];
    
    // and does it match?
    return valueMatches(obj, key, s[1]);

}

Value *getChildWithCondition(Object *obj, const string &condition) {

	for (vector<Pair>::iterator i=obj->begin(); i != obj->end(); i++) {
    	BOOST_LOG_TRIVIAL(debug) << "searching: " << i->name_ << " for " << condition;
		if (childMatches(&i->value_, condition)) {
    		BOOST_LOG_TRIVIAL(debug) << "found.";
			return &i->value_;
		}
	}
	return NULL;	

}

void dump(Value *v) {

	stringstream s;
	write_stream(*v, s, remove_trailing_zeros);
	BOOST_LOG_TRIVIAL(debug) << s.str();

}

Value *JsonPath::getPath(Value *obj, const string &path) {

	string remain = path;
	Value *o = obj;
	while (!remain.empty()) {
	
		switch (remain[0]) {
		case '.':
			{
				remain = remain.substr(1);
				string token = nextToken(&remain);
				o = get(&o->get_obj(), token);
				if (o == NULL || o->type() != obj_type) {
					throw runtime_error("token returned a non-object '" + token + "'");
				}
			}
			break;
			
		case '[':
			{
				remain = remain.substr(1);
				string condition = getCondition(&remain);
				if (condition.find_first_of("=") == string::npos) {
					if (o->type() != array_type) {
						throw runtime_error("tried to take index of non-array");
					}
					else {
						int index = lexical_cast<int>(condition);
						Array *a = &o->get_array();
						o = &a->at(index);
//    					BOOST_LOG_TRIVIAL(debug) << "got array object";
//						dump(o);
					}
				}
				else {
					o = getChildWithCondition(&o->get_obj(), condition);
				}
			}
			break;
			
		default:
			{
				string token = nextToken(&remain);
				o = get(&o->get_obj(), token);
			}
		}
	}
	
	return o;

}

JsonObject JsonPath::getPath(const JsonObject &obj, const string &path) const {

	Value *o = const_cast<JsonPath *>(this)->getPath(const_cast<Value *>(&obj._value), path);
	return JsonObject(*o);

}

void JsonPath::setPathInt(JsonObject *obj, const string &path, const string &var, int n) {

	Value *o = getPath(&obj->_value, path);
	Object *vo = &o->get_obj();
	for (vector<Pair>::iterator i=vo->begin(); i != vo->end(); i++) {
		if (var == i->name_) {
			i->value_ = Value(n);
		}
	}
	
}

