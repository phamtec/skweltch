#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "../runner.hpp"
#include "../JsonConfig.hpp"
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

	Runner r(&er);
	stringstream pids("1\n2\n");
	r.stopBackground(&pids);

}

MOCK_BASE_CLASS( mock_file_task, IFileTask )
{
	MOCK_METHOD( process, 1 )
};

BOOST_AUTO_TEST_CASE( pidFileNotExistTest )
{
	mock_file_task f;
	
	MOCK_EXPECT(f.process).never().with(mock::any);

	FileProcessor fp(&f);
	fp.processFileIfExistsThenDelete("test.pids");

}

BOOST_AUTO_TEST_CASE( pidFileExistsTest )
{
	mock_file_task f;
	
	MOCK_EXPECT(f.process).once().with(mock::any);

	// write the test file out.
	{
		ofstream f("test.pids");
		f << "1\n\2";
	}
	
	// process it, should delete the file.
	FileProcessor fp(&f);
	fp.processFileIfExistsThenDelete("test.pids");

	// make sure it's gone.
	{
		ifstream f("test.pids");
		BOOST_CHECK(!f.is_open());
	}
}

