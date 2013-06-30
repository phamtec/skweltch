#define BOOST_TEST_MODULE vent-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonNode.hpp"
#include "../Vent.hpp"
#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
    MOCK_METHOD_EXT( send, 2, bool(zmq::message_t &, int), send )
   	MOCK_METHOD_EXT( recv, 2, bool(zmq::message_t *, int), recv )
};

MOCK_BASE_CLASS( mock_builder, IVentMsgBuilder )
{
	MOCK_METHOD( init, 0 )
	MOCK_METHOD( buildMessage, 2 )
};

BOOST_AUTO_TEST_CASE( ventTest )
{
	mock_socket sender;
	mock_builder b;
	
	MOCK_EXPECT(sender.send).with(mock::any, 0).exactly(2).returns(true);
	MOCK_EXPECT(b.init).once();
	MOCK_EXPECT(b.buildMessage).with(mock::any, mock::any).exactly(2);

    Vent s(&b);
    
    stringstream json("{\"count\": 2}");
	JsonConfig c(&json);
	JsonNode root;
	BOOST_CHECK(c.read(&root, &std::cout));
    
    s.service(&root, &sender);
    
}
