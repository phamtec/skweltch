
#include "JsonWriter.hpp"
#include <climits>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

#define START_END_STRING	"\""
#define START_OBJECT		"{"
#define END_OBJECT			"}"
#define END_KEY				":"
#define SEP_OBJECT			","
#define TAB					"    "
#define SPACE				" "

void JsonWriter::writeData(const any_ptree &pt, ostream *ss) {

	// since it's any, we can just go ahead and work out it's type.
	any x = pt.get<any>("", any());
	if (x.type() == typeid(int)) {
		writeInt(any_cast<int>(x), ss);
	}
	else if (x.type() == typeid(double)) {
		writeDouble(any_cast<double>(x), ss);
	}
	else {
		writeString(any_cast<string>(x), ss);
	}
}

void JsonWriter::writeString(const string &s, ostream *ss) {
	*ss << START_END_STRING << s << START_END_STRING;
}

void JsonWriter::writeInt(int i, ostream *ss) {
	*ss << i;
}

void JsonWriter::writeDouble(double d, ostream *ss) {
	*ss << d;
}

void JsonWriter::writeKey(const string &k, ostream *s) {
	writeString(k, s);
	*s << END_KEY;
}

void JsonWriter::writeNode(const any_ptree &pt, ostream *s, int level) {
	if (prettyPrint) {
		if (level > 0) {
			*s << endl;
		}
		for (int i=0; i<level; i++) {
			*s << TAB;
		}
	}
	*s << START_OBJECT;
	
	for (any_ptree::const_iterator i=pt.begin(); i != pt.end(); i++) {
	
		if (i != pt.begin()) {
			*s << SEP_OBJECT;
		}
		if (prettyPrint) {
			*s << endl;
			for (int i=0; i<level+1; i++) {
				*s << TAB;
			}
		}
		
		if (i->second.empty()) {
			writeKey(i->first, s);
			if (prettyPrint) {
				*s << SPACE;
			}
			writeData(i->second, s);
		}
		else {
			writeKey(i->first, s);
			writeNode(i->second, s, level+1);
		}
		
	}
	if (prettyPrint) {
		*s << endl;
		for (int i=0; i<level; i++) {
			*s << TAB;
		}
	}
	*s << END_OBJECT;
}

void JsonWriter::write(const any_ptree &pt, ostream *s) {

	writeNode(pt, s, 0);
	
}
