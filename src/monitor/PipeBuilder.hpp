
#ifndef __PIPEBUILDER_HPP_INCLUDED__
#define __PIPEBUILDER_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>

class PipeBuilder {

public:

	JsonObject collect(JsonObject *config, const JsonObject &block);

};

#endif // __PIPEBUILDER_HPP_INCLUDED__

