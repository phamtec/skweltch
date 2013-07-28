
#ifndef __MACHINEGRAPH_HPP_INCLUDED__
#define __MACHINEGRAPH_HPP_INCLUDED__

#include <iostream>
#include <log4cxx/logger.h>

class JsonObject;

/**
	Produce a graph of the machine. Probably with for/with dot.
*/

class MachineGraph  {

	log4cxx::LoggerPtr logger;
	std::ostream *dot;
	
	void writeNode(const JsonObject &node);
	void writeNodeInfo(const JsonObject &node);
	void writeNodeConfig(const JsonObject &node);
	void writeNodeNameRow(const std::string &name);
	void writeNodeInfoRow(const std::string &name, const std::string &value);
	void writeNodeConfigRow(const std::string &name, const std::string &value);
	void writeEdge(const std::string &from, const std::string &to, 
		const std::string &fromlabel, const std::string &tolabel, bool samehead);
	void writeTableHeader();
	
public:
	MachineGraph(log4cxx::LoggerPtr l, std::ostream *d) : logger(l), dot(d) {}
	
	bool makeDOT(std::istream *json);
};

#endif // __MACHINEGRAPH_HPP_INCLUDED__

