
#include "BoostAnalyser.hpp"

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

typedef enum {
		START = 0,
		BUILDING = 1,
		COMPILING = 2,
		LINKING = 3,
		TESTING = 4,
		TESTRUN = 5,
		FINISHED = 6
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

BuildStatus BoostAnalyser::analyse(istream *stream) {

	State state = START;
	
	// the regex's that make up a boost build log.
	regex reUpdating("\\.\\.\\.updating (.*) targets\\.\\.\\.");
	regex reUpdated("\\.\\.\\.updated (.*) targets\\.\\.\\.");
	regex reCompile("(.*)\\.compile\\.c\\+\\+ (.*)");
	regex reLink("(.*)\\.link (.*)");
	regex reTesting("testing\\.unit-test (.*)");
	regex reRunning("Running (.*) test case[s]*\\.\\.\\.");
	regex reGood("\\*\\*\\* No errors detected");
	regex reBad("\\*\\*\\* (.*) failure detected in test suite");

	string l;
	BuildStatus status;
	status.targets = 0;
	status.success = false;
	status.workDone = false;
	status.passedTests = 0;
	match_results<std::string::const_iterator> what;
	while (state != FINISHED && getline(*stream, l)) {
		switch (state) {
		case START: 
			matchOne(l, &reUpdating, &state, BUILDING, &status.targets);
			break;
			
		case BUILDING:
			status.workDone = true;
			if (!matchOne(l, &reCompile, &state, COMPILING)) {
				if (matchOne(l, &reUpdated, &state, FINISHED)) {
					status.success = true;
				}
			}
			break;
			
		case COMPILING:
			if (!matchOne(l, &reLink, &state, LINKING)) {
				matchOne(l, &reCompile, &state, COMPILING);
			}
			break;
			
		case LINKING:
			matchOne(l, &reTesting, &state, TESTING);
			break;
			
		case TESTING:
			matchOneInc(l, &reRunning, &state, TESTRUN, &status.passedTests);
			break;

		case TESTRUN:
			if (!matchOne(l, &reGood, &state, BUILDING)) {
				matchOneDec(l, &reBad, &state, FINISHED, &status.passedTests);
			}
			break;
			
		case FINISHED:
			break;
		}		
	}
	
	return status;
}
