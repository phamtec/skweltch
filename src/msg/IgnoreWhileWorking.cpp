
#include "IgnoreWhileWorking.hpp"

#include "IDataMsg.hpp"
#include "IClock.hpp"

void IgnoreWhileWorking::start(IDataMsg *msg) {

	remoteStart = msg->getTime();
	localStart = clock->time();
}

void IgnoreWhileWorking::end() {

	localEnd = clock->time();
	
}

bool IgnoreWhileWorking::canIgnore(IDataMsg *msg) {

	if (remoteStart == 0 || localEnd == 0 || localStart == 0) {
		return false;
	}
	clock_t t = msg->getTime();
	return (t - remoteStart) < (localEnd - localStart);
	
}
	
