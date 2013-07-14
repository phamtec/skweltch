#define BOOST_TEST_MODULE json-tests
#include <boost/test/unit_test.hpp>

#include "../JsonObject.hpp"
#include "../JsonArray.hpp"
#include "../JsonPath.hpp"
#include "../JsonNamePredicate.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;
using namespace json_spirit;

void testWrite(const JsonObject &o, bool pp, const string &result) {

	ostringstream ss;
	o.write(pp, &ss);

//	cout << "[" << ss.str() << "]" << endl;
//	cout << "[" << result << "]" << endl;
	
	BOOST_CHECK(result == ss.str());

}

BOOST_AUTO_TEST_CASE( simpleTest )
{
	JsonObject o;
	o.add("x", "y");
	
	ostringstream ss;
	o.write(false, &ss);
    
	BOOST_CHECK(ss.str() == "{\"x\":\"y\"}");
	
	JsonObject v;
	BOOST_CHECK(v.read(ss.str()));
	BOOST_CHECK(v.getString("x") == "y");	
}

BOOST_AUTO_TEST_CASE( simpleIntTest )
{
	JsonObject o;
	o.add("x", 1);
	
	ostringstream ss;
	o.write(false, &ss);
    
	BOOST_CHECK(ss.str() == "{\"x\":1}");
	
	JsonObject v;
	BOOST_CHECK(v.read(ss.str()));
	BOOST_CHECK(v.getInt("x", 0) == 1);
	
}

BOOST_AUTO_TEST_CASE( objectTest )
{
	JsonObject o;
	JsonObject n;
	n.add("a", "b");
	o.add("x", n);
	
	testWrite(o, false, "{\"x\":{\"a\":\"b\"}}");
}

BOOST_AUTO_TEST_CASE( multiObjectTest )
{
	JsonObject o;
	JsonObject n;
	n.add("a", "b");
	n.add("c", "d");
	n.add("e", "f");
	o.add("x", n);
	
	testWrite(o, false, "{\"x\":{\"a\":\"b\",\"c\":\"d\",\"e\":\"f\"}}");
}

BOOST_AUTO_TEST_CASE( nestedObjectTest )
{
	JsonObject o;
	JsonObject n;
	n.add("a", "b");
	n.add("c", "d");
	JsonObject o2;
	o2.add("e", "f");
	n.add("x", o2);
	o.add("y", n);

	testWrite(o, false, "{\"y\":{\"a\":\"b\",\"c\":\"d\",\"x\":{\"e\":\"f\"}}}");
}

BOOST_AUTO_TEST_CASE( ppTest )
{

	JsonObject o;
	JsonObject n;
	n.add("a", "b");
	n.add("c", "d");
	JsonObject o2;
	o2.add("e", "f");
	n.add("x", o2);
	o.add("y", n);

string result = "{\n"
"    \"y\" : {\n"
"        \"a\" : \"b\",\n"
"        \"c\" : \"d\",\n"
"        \"x\" : {\n"
"            \"e\" : \"f\"\n"
"        }\n"
"    }\n"
"}";

	testWrite(o, true, result);

}

BOOST_AUTO_TEST_CASE( intTest )
{
	JsonObject o;
	JsonObject n;
	n.add("a", 1);
	n.add("c", 2);
	JsonObject o2;
	o2.add("e", 3);
	n.add("x", o2);
	o.add("y", n);

	testWrite(o, false, "{\"y\":{\"a\":1,\"c\":2,\"x\":{\"e\":3}}}");
}

BOOST_AUTO_TEST_CASE( doubleTest )
{
	JsonObject o;
	JsonObject n;
	n.add("a", 8.2);
	n.add("c", 9.3);
	JsonObject o2;
	o2.add("e", 10.4);
	n.add("x", o2);
	o.add("y", n);

	testWrite(o, false, "{\"y\":{\"a\":8.199999999999999,\"c\":9.300000000000001,\"x\":{\"e\":10.4}}}");

}

BOOST_AUTO_TEST_CASE( arrayTest )
{
	string json = "{\"data\":[{\"x\": 1},{\"x\":2}]}";
	JsonObject o;
	BOOST_CHECK(o.read(json));
	JsonArray a = o.getArray("data");
	JsonArray::iterator i = a.begin();
	BOOST_CHECK(i != a.end());
	BOOST_CHECK(a.getInt(i, "x") == 1);
	i++;
	BOOST_CHECK(a.getInt(i, "x") == 2);
	i++;
	BOOST_CHECK(i == a.end());

}

BOOST_AUTO_TEST_CASE( findByNameTest )
{
	string json = "{\n"
"	\"aaa\": [\n"
"		{\n"
"			\"name\": \"Block1\",\n"
"			\"value\": 1\n"
"		},\n"
"		{\n"
"			\"name\": \"Block2\",\n"
"			\"value\": \"xxx\"\n"
"		}\n"
"	],\n"
"	\"bbb\": {\n"
"		\"name\": \"Block3\",\n"
"		\"value\": \"yyy\"\n"
"	}\n"
"}\n";

	JsonObject obj;
	BOOST_CHECK(obj.read(json));
	BOOST_CHECK(obj.findObj(JsonNamePredicate("Block2")).getString("value") == "xxx");
	BOOST_CHECK(obj.findObj(JsonNamePredicate("Block1")).getInt("value", -1) == 1);
	BOOST_CHECK(obj.findObj(JsonNamePredicate("Block3")).getString("value") == "yyy");
}

BOOST_AUTO_TEST_CASE( writeArrayTest )
{
	JsonObject obj;
	JsonArray aaa;
	JsonObject o;
	o.add("x", "y");
	aaa.add(o);
	JsonObject p;
	p.add("a", "b");
	aaa.add(p);
	obj.add("aaa", aaa);
	
	ostringstream ss;
	obj.write(false, &ss);

	BOOST_CHECK(ss.str() == "{\"aaa\":[{\"x\":\"y\"},{\"a\":\"b\"}]}");

}
