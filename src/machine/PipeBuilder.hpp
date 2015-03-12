
#ifndef __PIPEBUILDER_HPP_INCLUDED__
#define __PIPEBUILDER_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>
#include <log4cxx/logger.h>

class PipeBuilder {

	log4cxx::LoggerPtr logger;

public:
	PipeBuilder(log4cxx::LoggerPtr l) : logger(l) {}

	JsonObject collect(JsonObject *config, const JsonObject &block, int control);

};

#endif // __PIPEBUILDER_HPP_INCLUDED__

