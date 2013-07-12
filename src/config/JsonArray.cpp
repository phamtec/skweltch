
#include "JsonArray.hpp"

#include "JsonObject.hpp"

#include "json_spirit.h"
#include "json_spirit_stream_reader.h"
#include "json_spirit_writer_template.h"

using namespace std;
using namespace boost;
using namespace json_spirit;

int JsonArray::getInt(iterator it, const string &name) const {

	Value v = JsonObject::get(it->get_obj(), name);
	if (v.type() == null_type) {
		return 0;
	}
	return v.get_value<int>();
	
}

string JsonArray::getString(iterator it, const string &name) const {

	Value v = JsonObject::get(it->get_obj(), name);
	if (v.type() == null_type) {
		return "";
	}
	return v.get_value<string>();
	
}

string JsonArray::getChildAsString(iterator it, const string &name) const {

	JsonObject o;
	o._value = JsonObject::get(it->get_obj(), name);
	ostringstream ss;
	o.write(false, &ss);
	return ss.str();
	
}

JsonObject JsonArray::getValue(iterator it) const {

	JsonObject o;
	*o.getValue() = *it;
	return o;

}

void JsonArray::add(JsonObject *o) {

	_array.push_back(*o->getValue());
	
}


