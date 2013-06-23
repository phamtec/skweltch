
#ifndef __RUNNER_HPP_INCLUDED__
#define __RUNNER_HPP_INCLUDED__

#include <fstream>
#include <boost/property_tree/ptree.hpp>

void startBackground(std::ofstream *pidfile, int n, const std::string &exe);
void startBackground(std::ofstream *pidfile, const std::string &exe);
void stopBackground(const std::string &pidfilename);
int runExe(const std::string &exe);
void readConfig(const std::string &jsonfilename, boost::property_tree::ptree *pt);

#endif // __RUNNER_HPP_INCLUDED__
