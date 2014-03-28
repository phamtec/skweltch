
#ifndef __GIT_HPP_INCLUDED__
#define __GIT_HPP_INCLUDED__

#include <log4cxx/logger.h>
#include <string>

typedef struct git_repository git_repository;

/*
	Class used to poll git
*/

class Git {

private:
	log4cxx::LoggerPtr logger;

public:
	Git(log4cxx::LoggerPtr l) : logger(l) {}
	
    bool poll(const std::string &folder, const std::string &repos);

private:
    bool shellPull(const std::string &folder);
    int nativePull(git_repository *repo);

};

#endif // __GIT_HPP_INCLUDED__

