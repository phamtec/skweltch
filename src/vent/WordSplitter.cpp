
#include "WordSplitter.hpp"

#include <boost/algorithm/string.hpp>  

using namespace std;
using namespace boost::algorithm;

bool WordSplitter::process(IWord *w) {

	while (!input->eof()) {
		string s;
		*input >> s;
		if (!std::isalnum(s[s.length()-1])) {
			s = s.substr(0, s.length()-1);
		}
		to_lower(s);
		if (!w->word(s)) {
			return false;
		}
	}
	return true;
}

