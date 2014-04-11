
#include "MsgTracker.hpp"

using namespace std;

void MsgTracker::track(int n) {

	int bkt = (n-first) / SIZE;
	int idx = (n-first) % SIZE;
	
	// push the extra buckets.
	grow(bkt+1);
	
	bits[bkt][idx] = true;
}

void MsgTracker::reset() {

	bits.clear();
	first = -1;
	last = -1;
    
}

void MsgTracker::grow(int n) {

	int k = n - bits.size();
	if (k > 0) {
		LOG4CXX_DEBUG(logger, "growing by " << k << " buckets.")
		for (int i=0; i<k; i++) {
			bits.push_back(bitset<SIZE>());
		}
	}
	
}

void MsgTracker::setFirst(int f)
{
    if (first != -1) {
		LOG4CXX_ERROR(logger, "first already set to " << first)
    }
    
    first = f;
}

void MsgTracker::setLast(int l) { 

    if (last != -1) {
		LOG4CXX_ERROR(logger, "last already set to " << last)
    }
    
 	last = l;
	
	// grow to n buckets.
	int bkt = (last-first) / SIZE;
	grow(bkt+1);

}

bool MsgTracker::complete() {

    // jf we never started then we are completed.
    if (first == -1 && last == -1) {
        return true;
    }
    
	if (first == -1) {
		return false;
	}
	
	// don't know yet.
	if (last == -1) {
 		LOG4CXX_TRACE(logger, "no last yet.")
		return false;
	}
	
    if (bits.size() == 0) {
 		LOG4CXX_DEBUG(logger, "no messages to check.")
		return true;
    }
    
 	LOG4CXX_TRACE(logger, "testing complete buckets.")
 	
	// make sure all the lower buckets are full.
	for (size_t i=0; i<bits.size()-1; i++) {
		if (bits[i].count() != SIZE) {
  			LOG4CXX_TRACE(logger, "bucket " << i << " not complete.")
			return false;
		}
	}
	
 	LOG4CXX_TRACE(logger, "testing last bucket...")
 	
	// individually test the highest bucket.
	size_t idx = (last-first) % SIZE;
	for (size_t i=0; i<=idx; i++) {
		if (!bits[bits.size()-1].test(i)) {
  			LOG4CXX_TRACE(logger, "bit " << i << " in last bucket not complete.")
			return false;
		}
	}
	
  	LOG4CXX_INFO(logger, "all messages received.")
	return true;
}

void MsgTracker::dump() {
    
    LOG4CXX_INFO(logger, "first: " << first)
    LOG4CXX_INFO(logger, "last: " << last)
    LOG4CXX_INFO(logger, "bitsize: " << bits.size())
    LOG4CXX_INFO(logger, "complete? " << complete())
    
    size_t idx = (last-first) % SIZE;
	for (size_t i=0; i<bits.size(); i++) {
		if (bits[i].count() != SIZE) {
  			LOG4CXX_INFO(logger, "bucket " << i << " not complete.")
            int count = 0;
            for (size_t j=0; j<=idx; j++) {
                if (!bits[i].test(j)) {
                    count++;
                }
            }
            LOG4CXX_INFO(logger, count << " bits in the bucket not complete.")
		}
	}
}
