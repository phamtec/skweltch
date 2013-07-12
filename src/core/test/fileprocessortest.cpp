#define BOOST_TEST_MODULE fileprocessor-tests
#include <boost/test/unit_test.hpp>

#include "../FileProcessor.hpp"
#include "../IFileTask.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

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
