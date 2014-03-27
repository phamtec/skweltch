
#include "MakeAnalyser.hpp"

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

typedef enum {
		START = 0,
        BUILDING = 1,
		INSTALLING = 2,
		TESTING = 3,
		FINISHED = 4
} State;

bool matchOne(const string &l, regex *re, State *state, State next) {

	match_results<std::string::const_iterator> what;
	bool result = regex_search(l, what, *re);
	if (result) {
		*state = next;
	}
	return result;
}

template <typename T>
bool matchOne(const string &l, regex *re, State *state, State next, T *val) {

	match_results<std::string::const_iterator> what;
	bool result = regex_search(l, what, *re);
	if (result) {
		*val = lexical_cast<T>(string(what[1].first, what[1].second));
		*state = next;
	}
	return result;
}

template <typename T>
bool matchOneInc(const string &l, regex *re, State *state, State next, T *val) {

	match_results<std::string::const_iterator> what;
	bool result = regex_search(l, what, *re);
	if (result) {
		*val += lexical_cast<T>(string(what[1].first, what[1].second));
		*state = next;
	}
	return result;
}

template <typename T>
bool matchOneDec(const string &l, regex *re, State *state, State next, T *val) {

	match_results<std::string::const_iterator> what;
	bool result = regex_search(l, what, *re);
	if (result) {
		*val -= lexical_cast<T>(string(what[1].first, what[1].second));
		*state = next;
	}
	return result;
}

BuildStatus MakeAnalyser::analyse(istream *stream) {

	BuildStatus status;
	status.success = false;
	status.workDone = false;

	State state = START;

	// the regex's that make up a make build log.
	regex reBuilding(" Built target ");
	regex reInstalling("Install the project...");
    regex reTesting("Running tests...");
	regex reGood("100% tests passed, 0 tests failed out of (.*)");
	regex reBad("(.*) tests passed, (.*) tests failed out of (.*)");

	string l;
	match_results<std::string::const_iterator> what;
	while (state != FINISHED && getline(*stream, l)) {
		LOG4CXX_TRACE(logger, state << ", " << l)
		switch (state) {
            case START:
                matchOne(l, &reBuilding, &state, BUILDING);
                break;
                
            case BUILDING:
                status.workDone = true;
                matchOne(l, &reInstalling, &state, INSTALLING);
                break;
                
            case INSTALLING:
                matchOne(l, &reTesting, &state, TESTING);
                break;
                
            case TESTING:
                if (matchOne(l, &reGood, &state, FINISHED)) {
 					status.success = true;
                }
                else if (matchOne(l, &reBad, &state, FINISHED)) {
                }
                break;
                
            case FINISHED:
                break;
		}		
	}
    
	return status;
}
