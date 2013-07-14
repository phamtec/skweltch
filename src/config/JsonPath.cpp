
#include "JsonPath.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
 
using namespace std;
using namespace boost;

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

bool childMatches(const JsonObject &obj, const string &condition) {

	// nvp
	vector<string> s;
    split(s, condition, is_any_of("="));
    
    // split possible quotes off key.
    string key = (s[0][0] == '"') ? s[0].substr(1, s[0].length()-2) : s[0];
    
    // and does it match?
    return obj.valueMatches(key, s[1]);

}

JsonObject getChildWithCondition(JsonObject *obj, const string &condition) {

	for (JsonObject::iterator i=obj->begin(); i != obj->end(); i++) {
    	BOOST_LOG_TRIVIAL(debug) << "searching: " << obj->getKey(i) << " for " << condition;
		JsonObject o = obj->getValue(i);
		if (childMatches(o, condition)) {
    		BOOST_LOG_TRIVIAL(debug) << "found.";
			return o;
		}
	}
	return JsonObject();	

}

JsonObject JsonPath::getPath(const JsonObject &obj, const string &path) const {

	string remain = path;
	JsonObject o = obj;
	while (!remain.empty()) {
	
		switch (remain[0]) {
		case '.':
			{
				remain = remain.substr(1);
				string token = nextToken(&remain);
				o = o.getChild(token);
				if (!o.isObject()) {
					throw runtime_error("token returned a non-object '" + token + "'");
				}
			}
			break;
			
		case '[':
			{
				remain = remain.substr(1);
				string condition = getCondition(&remain);
				if (condition.find_first_of("=") == string::npos) {
					if (!o.isArray()) {
						throw runtime_error("tried to take index of non-array");
					}
					else {
						int index = lexical_cast<int>(condition);
						JsonArray a = o.asArray();
						o = a.getValue(index);
    					BOOST_LOG_TRIVIAL(debug) << "got array object";
						o.dump();
					}
				}
				else {
					o = getChildWithCondition(&o, condition);
				}
			}
			break;
			
		default:
			{
				string token = nextToken(&remain);
				o = o.getChild(token);
			}
		}
	}
	return o;

/*	
	vector<string> s;
    split(s, path, is_any_of("."));
    
    JsonObject o = obj;
    for (vector<string>::iterator i=s.begin(); i != s.end(); i++) {
    	o = o.getChild(*i);
    }
	return o;
*/

}
