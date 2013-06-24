#define BOOST_TEST_MODULE sink-tests
#include <boost/test/unit_test.hpp>

#include "../JsonConfig.hpp"
#include "../Sink.hpp"
#include "../Messager.hpp"
#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_messager, IMessager )
{
	MOCK_METHOD( send, 1 )
	MOCK_METHOD( receive, 1 )
};

BOOST_AUTO_TEST_CASE( sinkTest )
{
	mock_messager m;
	
	MOCK_EXPECT(m.receive).with(mock::any).exactly(6);

    Sink s(&m, &cout);
    
    stringstream json("{\"expect\": 5}");
	JsonConfig c(&json);
	JsonNode root;
	c.read(&root);
    
    s.service(&root);
    
}
