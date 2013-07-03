
#include "JsonConfig.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>

using namespace std;
using namespace boost;

bool JsonConfig::read(property_tree::ptree *root, ostream *outfile) {

	try
    {
  		property_tree::read_json(*jsonstream, *root);
  		return true;
    }
    catch(property_tree::json_parser::json_parser_error &je)
    {
        *outfile << "Error parsing: " << je.filename() << " on line: " << je.line() << std::endl;
        *outfile << je.message() << std::endl;
        return false;
    }
}

std::string JsonConfig::getChildAsString(const property_tree::ptree &pt, const std::string &key) {
	boost::property_tree::ptree c = pt.get_child(key);
	ostringstream ss;
	property_tree::write_json(ss, c, false);
	string s = ss.str();
	trim(s);           
	return s;
}
