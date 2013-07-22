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
#include <log4cxx/basicconfigurator.h>

using namespace std;
using namespace boost;
using namespace json_spirit;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( jsonTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

JsonObject createObject() {

	// {"x":{"a":{"b":1}},"y":{"d":{"e":2},"g":{"h":3,"o":{"p":9}},"z":[{"i"5},{"j":6},{"l":{"m":7,"n":8}}]}
	JsonObject o;
	{
		JsonObject x;
		{
			JsonObject a;
			a.add("b", 1);
			x.add("a", a);
		}
		o.add("x", x);
	}
	{
		JsonObject y;
		{
			JsonObject d;
			d.add("e", 2);
			y.add("d", d);
		}
		{
			JsonObject g;
			g.add("h", 3);
			JsonObject oo;
			oo.add("p", 9);
			g.add("o", oo);
			y.add("g", g);
		}
		o.add("y", y);	
	}
	{
		JsonArray z;
		{
			JsonObject i;
			i.add("i", 5);
			z.add(i);
		}
		{
			JsonObject j;
			j.add("j", 6);
			z.add(j);
		}
		{
			JsonObject l;
			JsonObject s;
			s.add("m", 7);
			s.add("n", 8);
			l.add("l", s);
			z.add(l);
		}
		o.add("z", z);	
	}
	
	return o;
}

BOOST_AUTO_TEST_CASE( simpleTest )
{
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getChild("d").getInt("e", -1) == 2);

}

BOOST_AUTO_TEST_CASE( dotTest )
{
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y.d");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getInt("e", -1) == 2);

}

BOOST_AUTO_TEST_CASE( badDotTest )
{
	JsonObject o = createObject();

	try {
		JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y.");
		BOOST_CHECK(false);
	}
	catch (runtime_error &e) {
		BOOST_CHECK(string(e.what()) == "token returned a non-object ''");
	}

}

BOOST_AUTO_TEST_CASE( objNotFoundTest )
{
	JsonObject o = createObject();

	try {
		JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y.aa");
		BOOST_CHECK(false);
	}
	catch (runtime_error &e) {
		BOOST_CHECK(string(e.what()) == "token returned a non-object 'aa'");
	}

}

BOOST_AUTO_TEST_CASE( conditionTest )
{	
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y[\"h\"=3]");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getInt("h", -1) == 3);
	
}

BOOST_AUTO_TEST_CASE( badConditionTest )
{	
	JsonObject o = createObject();

	try {
		JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "z[1");
		BOOST_CHECK(false);
	}
	catch (runtime_error &e) {
		BOOST_CHECK(string(e.what()) == "no end for condition");
	}
	
}

BOOST_AUTO_TEST_CASE( conditionSubObjTest )
{	
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y[\"h\"=3].o");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getInt("p", -1) == 9);
	
}

BOOST_AUTO_TEST_CASE( arrayTest )
{	
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "z[1]");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getInt("j", -1) == 6);
	
}

BOOST_AUTO_TEST_CASE( badArrayTest )
{	
	JsonObject o = createObject();

	try {
		JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "y[1]");
		BOOST_CHECK(false);
	}
	catch (runtime_error &e) {
		BOOST_CHECK(string(e.what()) == "tried to take index of non-array");
	}
	
}

BOOST_AUTO_TEST_CASE( arraySubObjTest )
{	
	JsonObject o = createObject();

	JsonObject p = JsonPath(log4cxx::Logger::getRootLogger()).getPath(o, "z[2].l");
	BOOST_CHECK(p.isObject());
	BOOST_CHECK(p.getInt("m", -1) == 7);
	
}

BOOST_AUTO_TEST_SUITE_END()

