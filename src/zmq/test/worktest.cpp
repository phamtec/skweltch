#define BOOST_TEST_MODULE work-tests
#include <boost/test/unit_test.hpp>

#include "../JsonConfig.hpp"
#include "../Work.hpp"
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

MOCK_BASE_CLASS( mock_work, IWorkMsgExecutor )
{
	MOCK_METHOD( init, 0 )
	MOCK_METHOD( doit, 1 )
};

BOOST_AUTO_TEST_CASE( workTest )
{
	mock_socket sender;
	mock_socket receiver;
	mock_work w;
	
	MOCK_EXPECT(receiver.recv).with(mock::any, 0).once().returns(true);
	MOCK_EXPECT(sender.send).with(mock::any, 0).once().returns(true);
	MOCK_EXPECT(w.init).once();
	MOCK_EXPECT(w.doit).with(mock::any).exactly(1);
    
    stringstream json("{}");
	JsonConfig c(&json);
	JsonNode root;
	BOOST_CHECK(c.read(&root, &std::cout));
    
   	Work s(&w);
    s.service(&root, &receiver, &sender, false);
    
}
