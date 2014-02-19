#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../Vent.hpp"
#include "../IVentWorker.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( ventTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( recv, 2 )
	MOCK_METHOD( bind, 1 )
	MOCK_METHOD( connect, 1 )
	MOCK_METHOD( setsockopt, 3 )
};

MOCK_BASE_CLASS( mock_vent_worker, IVentWorker )
{
	MOCK_METHOD( sendAll, 1 )
	MOCK_METHOD( shouldQuit, 0 )
};

BOOST_AUTO_TEST_CASE( simpleTest )
{
	mock_socket sink;
	mock_socket sender;
	mock_vent_worker w;
	
	MOCK_EXPECT(sink.send).with(mock::any, 0).exactly(2).returns(true);
	MOCK_EXPECT(sender.send).with(mock::any, 0).never();
	MOCK_EXPECT(w.sendAll).with(mock::any).once().returns(true);
	
	Vent v(log4cxx::Logger::getRootLogger(), &sink, &sender);
	BOOST_CHECK(v.process(&w));
}

BOOST_AUTO_TEST_CASE( sendTest )
{
	mock_socket sink;
	mock_socket sender;
	mock_vent_worker w;
	
	MOCK_EXPECT(sink.send).with(mock::any, 0).never();
	MOCK_EXPECT(sender.send).with(mock::any, 0).once().returns(true);
	MOCK_EXPECT(w.sendAll).with(mock::any).never();
	MOCK_EXPECT(w.shouldQuit).once().returns(false);
	
	Vent v(log4cxx::Logger::getRootLogger(), &sink, &sender);
	
   	zmq::message_t message(2);
 	msgpack::type::tuple<int, int, int> wmsg(0, 1, 2);
 	msgpack::sbuffer sbuf;
   	msgpack::pack(sbuf, wmsg);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    
	BOOST_CHECK(v.sendOne(&w, message, 0, 0));
	
}

BOOST_AUTO_TEST_SUITE_END()
