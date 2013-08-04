
#include "XmlParser.hpp"

#include <boost/algorithm/string.hpp>  

using namespace std;
using namespace boost::algorithm;

bool XmlParser::process(IXmlElement *elem) {
	
	enum {
		START = 0,
		INPI = 1,
		INTAG = 2,
		INATTR = 3
	} state = START;
	
	string s = "";
	while (!input->eof()) {
	
		char c = input->get();
		
		switch (c) {
		
		case '<':
			switch (state) {
			case START:
				// dump what we have since the last end tag.
				if (!s.empty()) {
					if (!elem->content(s)) {
						return false;
					}
				}
				state = INTAG;
				s = "";
				break;
				
			case INPI:
			case INATTR:
			case INTAG:
				LOG4CXX_ERROR(logger, "bad state. nested <.")
				break;
				
			}
			break;
			
		case '?':
			switch (state) {
			case INTAG:
				state = INPI;
				break;
				
			default:
				s += c;	
			}
			break;
			
		case ' ':
			switch (state) {
			case START:
				s += c;
				break;
				
			case INATTR:
				// go to next attr.
				if (!elem->attr(s)) {
					return false;
				}
				s = "";
				break;
				
			case INTAG:
				state = INATTR;
				if (!elem->startTag(s)) {
					return false;
				}
				// collecting the attribute.
				s = "";
				break;
				
			default:
				s += c;
			}
			break;
						
		case '>':
			switch (state) {
			
			case START:
				LOG4CXX_ERROR(logger, "bad state. freestanding >.")
				return false;
				
			case INATTR:
				// output the last attribute.
				if (!elem->attr(s)) {
					return false;
				}
				if (!elem->endAttr()) {
					return false;
				}
				s = "";
				state = START;
				break;
				
			case INTAG:
				if (s[0] == '/') {
					if (!elem->endTag(s.substr(1))) {
						return false;
					}
				}
				else {
					if (!elem->startTag(s)) {
						return false;
					}
					if (!elem->endAttr()) {
						return false;
					}
				}
				s = "";
				state = START;
				break;
				
			case INPI:
				if (!elem->pi(s)) {
					return false;
				}
				s = "";
				state = START;
				break;
			}
			break;
			
		default:
			s += c;
		}
	}
	
	return true;
}

