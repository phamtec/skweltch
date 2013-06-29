#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "../runner.hpp"
#include "../JsonConfig.hpp"
#include "../IFileTask.hpp"
#include "../IExeRunner.hpp"
#include "../StopTasksFileTask.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( configTest )
{
	stringstream json(
"{\n"
"	\"pidFile\": \"pids.pid\",\n"
"	\"exePath\": \"./bin\",\n"
"	\n"
"	// these are all run in the background and remembered.\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 5, // need 5 of these. first arg to the program is the id.\n"
"			\"exe\": \"exe1\"\n"
"		},\n"
"		{\n"
"			// only 1 of these.\n"
"			\"exe\": \"exe2\"\n"
"		}\n"
"	],\n"
"	\n"
"	// this is what is used to feed the machine.\n"
"	\"run\": \"exe3\",\n"
"	\"config\" : {\n"
"		\"a\": \"x\",\n"
"		\"b\": \"y\"\n"
"	}\n"
"	\n"
"}\n"
);

	JsonConfig c(&json);
	JsonNode r;
	c.read(&r);

	BOOST_CHECK(r.getString("pidFile") == "pids.pid");
	BOOST_CHECK(r.getString("exePath") == "./bin");
	JsonNode bg;
	r.getChild("background", &bg);
	bg.start();
	BOOST_CHECK(bg.hasMore());
	BOOST_CHECK(bg.current()->getInt("count", 0) == 5);
	BOOST_CHECK(bg.current()->getString("exe") == "exe1");
	bg.next();
	BOOST_CHECK(bg.hasMore());
	BOOST_CHECK(bg.current()->getInt("count", 0) == 0);
	BOOST_CHECK(bg.current()->getString("exe") == "exe2");
	bg.next();
	BOOST_CHECK(!bg.hasMore());
	BOOST_CHECK(r.getString("run") == "exe3");
	BOOST_CHECK(r.getChildAsString("config") == "{\"a\":\"x\",\"b\":\"y\"}");
    
}

MOCK_BASE_CLASS( mock_exe_runner, IExeRunner )
{
	MOCK_METHOD( run, 1 )
	MOCK_METHOD( kill, 1 )

};

BOOST_AUTO_TEST_CASE( startTest )
{
	mock_exe_runner er;
	
	MOCK_EXPECT(er.run).with(mock::equal("exe1 ''")).returns(1);
	
	Runner r(&er);
	stringstream pids;
	r.startBackground(&pids, "exe1", "''");

}

BOOST_AUTO_TEST_CASE( startMultipleTest )
{
	mock_exe_runner er;
	
	MOCK_EXPECT(er.run).with(mock::equal("exe1 0 ''")).returns(1);
	MOCK_EXPECT(er.run).with(mock::equal("exe1 1 ''")).returns(1);
	
	Runner r(&er);
	stringstream pids;
	r.startBackground(&pids, 2, "exe1", "''");

}

BOOST_AUTO_TEST_CASE( stopTest )
{
	mock_exe_runner er;
	
	MOCK_EXPECT(er.kill).with(mock::equal(1));
	MOCK_EXPECT(er.kill).with(mock::equal(2));

	StopTasksFileTask r(&er);
	stringstream pids("1\n2\n");
	r.process(&pids);

}
