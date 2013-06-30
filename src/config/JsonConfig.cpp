
#include "JsonConfig.hpp"
#include "JsonNode.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>

using namespace std;
using namespace boost;

bool JsonConfig::read(JsonNode *root, ostream *outfile) {

	try
    {
  		property_tree::read_json(*jsonstream, root->pt);
  		return true;
    }
    catch(property_tree::json_parser::json_parser_error &je)
    {
        *outfile << "Error parsing: " << je.filename() << " on line: " << je.line() << std::endl;
        *outfile << je.message() << std::endl;
        return false;
    }
}
