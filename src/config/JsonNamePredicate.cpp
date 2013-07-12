
#include "JsonNamePredicate.hpp"

#include "JsonObject.hpp"

using namespace std;

bool JsonNamePredicate::match(const JsonObject &obj) const {

	if (!obj.isObject()) {
		return false;
	}
	
	return obj.getString("name") == name;
}
