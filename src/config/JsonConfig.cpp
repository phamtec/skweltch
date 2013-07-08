
#include "JsonConfig.hpp"
#include "JsonObject.hpp";

#include <iostream>

using namespace std;
using namespace boost;
using namespace json_spirit;

bool JsonConfig::read(JsonObject *root, std::ostream *outfile) {

	return root->read(jsonstream);
	
/*	try
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
*/
}
