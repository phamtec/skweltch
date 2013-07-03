
#ifndef __PORTS_HPP_INCLUDED__
#define __PORTS_HPP_INCLUDED__

#include <string>
#include <boost/property_tree/ptree.hpp>

class Ports {

public:

	std::string getBindSocket(const boost::property_tree::ptree &pipes, const boost::property_tree::ptree &root, const std::string &name);
	std::string getConnectSocket(const boost::property_tree::ptree &pipes, const boost::property_tree::ptree &root, const std::string &name);

};

#endif // __PORTS_HPP_INCLUDED__

