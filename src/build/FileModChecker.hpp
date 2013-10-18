
#ifndef __FILEMODCHECKERHPP_INCLUDED__
#define __FILEMODCHECKERHPP_INCLUDED__

#include <log4cxx/logger.h>

/*
	Class used to test for file modifications in a dir.
*/

class FileModChecker {

	log4cxx::LoggerPtr logger;

public:
	FileModChecker(log4cxx::LoggerPtr l) : logger(l) {}
	
	long getCrc(const std::string &path);
	
};

#endif // __FILEMODCHECKERHPP_INCLUDED__

