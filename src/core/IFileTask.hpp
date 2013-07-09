
#ifndef __IFILETASK_HPP_INCLUDED__
#define __IFILETASK_HPP_INCLUDED__

/*
	Abstract interface for processing a file line by line.
*/

class IFileTask {

public:

	virtual void process(std::istream *stream) = 0;
	
};

#endif // __IFILETASK_HPP_INCLUDED__
