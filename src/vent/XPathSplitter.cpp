
#include "XPathSplitter.hpp"
#include "XmlParser.hpp"

#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace boost;

class XPathSplitterElem : public IXmlElement {

private:
	log4cxx::LoggerPtr logger;
	vector<string> stack;
	size_t spos;
	string chunk;
	IChunk *ch;
	
public:
	XPathSplitterElem(log4cxx::LoggerPtr l, const string &xpath, IChunk *c);
	
	virtual bool pi(const string &s) { return true; }
	virtual bool startTag(const string &s);
	virtual bool attr(const string &s);
	virtual bool endAttr();
	virtual bool endTag(const string &s);
	virtual bool content(const string &s);
	
};

XPathSplitterElem::XPathSplitterElem(log4cxx::LoggerPtr l, const string &xpath, IChunk *c) {

	logger = l;
	split(stack, xpath, is_any_of("/"));
	chunk = "";
	ch = c;
	spos = 0;
}

bool XPathSplitterElem::startTag(const string &s) {

	LOG4CXX_DEBUG(logger, "start tag " << s << ".")

    if (spos < stack.size()) {
		if (s == stack[spos]) {
			spos++;
			chunk = "";
		}
	}
	chunk += "<" + s;
	return true;
}

bool XPathSplitterElem::attr(const string &s) {

	chunk += " " + s;
	return true;
	
}

bool XPathSplitterElem::endAttr() {

	chunk += ">";
	return true;
	
}

bool XPathSplitterElem::endTag(const string &s) {

	LOG4CXX_DEBUG(logger, "end tag " << s << ".")

	chunk += "</" + s + ">";	
	
	if (spos > 0) {
		if (s == stack[spos-1]) {
		
			if (spos == stack.size()) {
				// finished the chunk.
				ch->chunk(chunk);
			}
					
			// back up the stack.
			spos--;
		}
	}
	
	return true;
}

bool XPathSplitterElem::content(const string &s) {

	chunk += s;
	return true;
	
}

bool XPathSplitter::process(const string &xpath, IChunk *ch) {
	
	XmlParser p(logger, input);
	XPathSplitterElem e(logger, xpath, ch);
	return p.process(&e);

}

