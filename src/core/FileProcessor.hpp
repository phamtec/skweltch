
#ifndef __FILEPROCESSOR_HPP_INCLUDED__
#define __FILEPROCESSOR_HPP_INCLUDED__

#include <string>

class IFileTask;

/*
	Process a file line by line.
*/

class FileProcessor {

private:
	IFileTask *task;
	
public:
	FileProcessor(IFileTask *t) : task(t) {}

	void processFileIfExistsThenDelete(const std::string &filename);
	
};

#endif // __FILEPROCESSOR_HPP_INCLUDED__
