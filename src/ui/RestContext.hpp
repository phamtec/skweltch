
#ifndef __RESTCONTEXT_HPP_INCLUDED__
#define __RESTCONTEXT_HPP_INCLUDED__

#include <vector>
#include "JsonObject.hpp"
#include "header.hpp"

/**
	A context for REST
*/
class RestContext {

public:

	static RestContext *getContext();

	void makeHeaders(std::vector<http::server::header> *headers, int size, const std::string &mimetype);
	bool load(const std::string &config);
	bool isLoaded() { return loaded; }
	void setLoaded() { loaded = true; }
	JsonObject *getRoot() { return &root; }
	
private:

	RestContext() : loaded(false) {}
	
	static RestContext *gContext;
	
	bool loaded;
	JsonObject root;
	
};

#endif // __RESTCONTEXT_HPP_INCLUDED__
