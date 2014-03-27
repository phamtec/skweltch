
#ifndef __MAIN_HPP_INCLUDED__
#define __MAIN_HPP_INCLUDED__

#include <log4cxx/logger.h>

class JsonObject;

bool setup_main(int argc, char *argv[], JsonObject *pipes, JsonObject *root, log4cxx::LoggerPtr *logger);

#endif // __MAIN_HPP_INCLUDED__
