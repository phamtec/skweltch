#define BOOST_TEST_MODULE config-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonArray.hpp"
#include "../nodesHandler.hpp"
#include "../../RestContext.hpp"

using namespace std;
using namespace boost;
using namespace http::server;

BOOST_AUTO_TEST_CASE( nodesTest )
{
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(nodesHandler(RestContext::getContext(), "", &headers, &content) == reply::no_content);

	stringstream json(
"{\n"
"	\"vent\" : {\n"
"		\"name\": \"xxx\",\n"
"		\"config\" : {\n"
"			\"connections\": {\n"
"				\"pushTo\": {\n"
"					\"direction\": \"to\",\n"
"					\"block\": \"yyy\"\n"
"				},\n"
"				\"syncTo\": {\n"
"					\"direction\": \"to\",\n"
"					\"block\": \"zzz\"\n"
"				}\n"
"			}\n"
"		}\n"
"	},\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 3,\n"
"			\"name\": \"yyy\",\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pullFrom\": {\n"
"						\"direction\": \"from\"\n"
"					},\n"
"					\"pushTo\": {\n"
"						\"direction\": \"to\",\n"
"						\"block\": \"zzz\"\n"
"					}\n"
"				}\n"
"			}\n"
"		},\n"
"		{\n"
"			\"name\": \"zzz\",\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pullFrom\": {\n"
"						\"direction\": \"from\"\n"
"					}\n"
"				}\n"
"			}\n"
"		}\n"
"	],\n"
"	// and this is what is used to reap.\n"
"	\"reap\": {\n"
"		\"name\": \"aaa\",\n"
"		\"config\" : {\n"
"			\"connections\": {\n"
"				\"pullFrom\": {\n"
"					\"direction\": \"from\"\n"
"				}\n"
"			}\n"
"		}\n"
"	}\n"
"}\n"
);

	JsonConfig c(&json);
	BOOST_CHECK(c.read(RestContext::getContext()->getRoot()));
	RestContext::getContext()->setLoaded();
	BOOST_CHECK(nodesHandler(RestContext::getContext(), "", &headers, &content) == reply::ok);
	
	{
		JsonObject pt;
  		BOOST_CHECK(pt.read(content));
  		JsonObject yyy = pt.getChild("yyy");
  		BOOST_CHECK(yyy.getInt("count", 0) == 3);
  		BOOST_CHECK(yyy.getString("type") == "background");
  		JsonObject rect = yyy.getChild("rect");
  		BOOST_CHECK(rect.getInt("left", 0) > 0);
  		BOOST_CHECK(rect.getInt("top", 0) > 0);
  		BOOST_CHECK(rect.getInt("width", 0) > 0);
  		BOOST_CHECK(rect.getInt("height", 0) > 0);
  		
  		JsonArray conn = pt.getArray("connections");
  		BOOST_CHECK(!conn.empty());
  		JsonArray::iterator i = conn.begin();
  		BOOST_CHECK(i != conn.end());
		BOOST_CHECK(conn.getString(i, "from") == "xxx");
		BOOST_CHECK(conn.getString(i, "to") == "yyy");
		i++;
  		BOOST_CHECK(i != conn.end());
		BOOST_CHECK(conn.getString(i, "from") == "xxx");
		BOOST_CHECK(conn.getString(i, "to") == "zzz");
		i++;
  		BOOST_CHECK(i != conn.end());
		BOOST_CHECK(conn.getString(i, "from") == "yyy");
		BOOST_CHECK(conn.getString(i, "to") == "zzz");
		i++;
   		BOOST_CHECK(i == conn.end());
	}
	
}
