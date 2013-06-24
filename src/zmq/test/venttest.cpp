#define BOOST_TEST_MODULE vent-tests
#include <boost/test/unit_test.hpp>

#include "../JsonConfig.hpp"
#include "../Vent.hpp"
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

MOCK_BASE_CLASS( mock_builder, IVentMsgBuilder )
{
	MOCK_METHOD( init, 0 )
	MOCK_METHOD( buildMessage, 2 )
};

BOOST_AUTO_TEST_CASE( ventTest )
{
	mock_messager m;
	mock_builder b;
	
	MOCK_EXPECT(m.send).with(mock::any).exactly(2);
	MOCK_EXPECT(b.init).once();
	MOCK_EXPECT(b.buildMessage).with(mock::any, mock::any).exactly(2);

    Vent s(&m, &b);
    
    stringstream json("{\"count\": 2}");
	JsonConfig c(&json);
	JsonNode root;
	c.read(&root);
    
    s.service(&root);
    
}
