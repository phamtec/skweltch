#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../WordSplitter.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_word, IWord )
{
	MOCK_METHOD( word, 1 )
};

BOOST_AUTO_TEST_CASE( startTest )
{
	string words = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed et "
"malesuada dui. Nulla tincidunt et ligula id pellentesque. Nullam "
"pellentesque justo sem, a elementum nunc lacinia eu.";

	istringstream ss(words);
	mock_word w;

	MOCK_EXPECT(w.word).exactly(27).with(mock::any).returns(true);
	
	WordSplitter splitter(&ss);
	BOOST_CHECK(splitter.process(&w));

}
