#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "../runner.hpp"
#include "../IFileTask.hpp"
#include "../IExeRunner.hpp"
#include "../StopTasksFileTask.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

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
