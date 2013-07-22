
#include "RestContext.hpp"

#include <iostream>
#include <fstream>

using namespace std;

RestContext *RestContext::gContext = 0;

RestContext *RestContext::getContext() {

	if (gContext == 0) {
		gContext = new RestContext();
	}
	return gContext;
	
}
	
bool RestContext::load(const std::string &config) {

	ifstream jsonfile(config.c_str());
	if (root.read(log4cxx::Logger::getRootLogger(), &jsonfile)) {
		loaded = true;
	}
	return loaded;
	
}
