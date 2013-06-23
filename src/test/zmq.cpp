#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include <zmq.hpp>

BOOST_AUTO_TEST_CASE( smoke1 )
{
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");
	
    BOOST_CHECK(true);
}
