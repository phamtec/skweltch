#define BOOST_TEST_MODULE json-tests
#include <boost/test/unit_test.hpp>

#include "../JsonWriter.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

any_ptree read(const string &s) {

	istringstream ss(s);
	any_ptree pt;
  	read_json(ss, pt);
  	return pt;
}

void testWrite(const any_ptree &pt, bool pp, const string &result) {

	ostringstream ss;
	JsonWriter w(pp);
	w.write(pt, &ss);

//	cout << "[" << ss.str() << "]" << endl;
//	cout << "[" << result << "]" << endl;
	
	BOOST_CHECK(result == ss.str());

}

BOOST_AUTO_TEST_CASE( simpleTest )
{
	any_ptree pt;
	pt.add("x", any(string("y")));

	ostringstream ss;
	JsonWriter w;
	w.write(pt, &ss);
	
	BOOST_CHECK(any_cast<string>(read(ss.str()).get<any>("x")) == "y");

}

BOOST_AUTO_TEST_CASE( objectTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(string("b")));
	pt.add_child("x", n);

	testWrite(pt, false, "{\"x\":{\"a\":\"b\"}}");

}

BOOST_AUTO_TEST_CASE( multiObjectTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(string("b")));
	n.add("c", any(string("d")));
	n.add("e", any(string("f")));
	pt.add_child("x", n);

	testWrite(pt, false, "{\"x\":{\"a\":\"b\",\"c\":\"d\",\"e\":\"f\"}}");

}

BOOST_AUTO_TEST_CASE( nestedObjectTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(string("b")));
	n.add("c", any(string("d")));
	any_ptree o;
	o.add("e", any(string("f")));
	n.add_child("x", o);
	pt.add_child("y", n);

	testWrite(pt, false, "{\"y\":{\"a\":\"b\",\"c\":\"d\",\"x\":{\"e\":\"f\"}}}");

}

BOOST_AUTO_TEST_CASE( ppTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(string("b")));
	n.add("c", any(string("d")));
	any_ptree o;
	o.add("e", any(string("f")));
	n.add_child("x", o);
	pt.add_child("y", n);

string result = "{\n"
"    \"y\":\n"
"    {\n"
"        \"a\": \"b\",\n"
"        \"c\": \"d\",\n"
"        \"x\":\n"
"        {\n"
"            \"e\": \"f\"\n"
"        }\n"
"    }\n"
"}";

	testWrite(pt, true, result);

}

BOOST_AUTO_TEST_CASE( intTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(1));
	n.add("c", any(2));
	any_ptree o;
	o.add("e", any(3));
	n.add_child("x", o);
	pt.add_child("y", n);

	testWrite(pt, false, "{\"y\":{\"a\":1,\"c\":2,\"x\":{\"e\":3}}}");

}

BOOST_AUTO_TEST_CASE( doubleTest )
{
	any_ptree pt;
	any_ptree n;
	n.add("a", any(8.2));
	n.add("c", any(9.3));
	any_ptree o;
	o.add("e", any(10.4));
	n.add_child("x", o);
	pt.add_child("y", n);

	testWrite(pt, false, "{\"y\":{\"a\":8.2,\"c\":9.3,\"x\":{\"e\":10.4}}}");

}
