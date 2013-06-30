#define BOOST_TEST_MODULE sink-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonNode.hpp"
#include "../Sink.hpp"
#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <zmq.hpp>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
    MOCK_METHOD_EXT( send, 2, bool(zmq::message_t &, int), send )
   	MOCK_METHOD_EXT( recv, 2, bool(zmq::message_t *, int), recv )
};

BOOST_AUTO_TEST_CASE( sinkTest )
{
	mock_socket receiver;
	
	MOCK_EXPECT(receiver.recv).with(mock::any, 0).exactly(6).returns(true);

    stringstream json("{\"expect\": 5}");
	JsonConfig c(&json);
	JsonNode root;
	BOOST_CHECK(c.read(&root, &std::cout));
    
    Sink s(&cout);
    s.service(&root, &receiver);
    
}
