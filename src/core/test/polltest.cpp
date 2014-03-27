#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../Poll.hpp"
#include "../IPollWorker.hpp"

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

BOOST_AUTO_TEST_SUITE( pollTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( recv, 2 )
	MOCK_METHOD( bind, 1 )
	MOCK_METHOD( connect, 1 )
	MOCK_METHOD( setsockopt, 3 )
};

MOCK_BASE_CLASS( mock_poll_worker, IPollWorker )
{
	MOCK_METHOD( waitEvent, 0 )
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( shouldQuit, 0 )
};

BOOST_AUTO_TEST_CASE( simpleTest )
{
	mock_socket sender;
	mock_poll_worker w;
	
	// we don't send in the worker.
	MOCK_EXPECT(sender.send).with(mock::any, 0).never();
	
	// we only wait once.
	MOCK_EXPECT(w.waitEvent).once().returns(true);
	
	// there is one send.
	MOCK_EXPECT(w.send).with(0, mock::any).once().returns(true);
	
	// we quit straight away
	MOCK_EXPECT(w.shouldQuit).once().returns(true);
	
	Poll p(log4cxx::Logger::getRootLogger(), &sender);
    
    // poll returns false when quitting.
	BOOST_CHECK(!p.process(&w));
}

class MockWorker: public IPollWorker {

private:
	int n;
	int count;
	
public:
	MockWorker(int c) : n(0), count(c) {}
	
	virtual bool waitEvent() { 
		n++;
		return true; 
	}
	
	virtual int send(int nextMsg, Poll *poll) { 
		zmq::message_t message(2);
		msgpack::type::tuple<int, int, int> wmsg(nextMsg++, 1, 2);
		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, wmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		poll->send(this, message, 0, 0);
		return nextMsg;
	}
	
	virtual bool shouldQuit() { 
		return (n == count);
	}
	
};

BOOST_AUTO_TEST_CASE( twoEventsTest )
{
	mock_socket sender;
	MockWorker w(3);
	
	// and we send 3 times.
	MOCK_EXPECT(sender.send).with(mock::any, 0).exactly(3).returns(true);
	
	Poll p(log4cxx::Logger::getRootLogger(), &sender);
	BOOST_CHECK(!p.process(&w));
}

BOOST_AUTO_TEST_CASE( sendTest )
{
	mock_socket sender;
	mock_poll_worker w;
	
	MOCK_EXPECT(sender.send).with(mock::any, 0).once().returns(true);
	MOCK_EXPECT(w.send).with(0, mock::any).never();
	MOCK_EXPECT(w.shouldQuit).once().returns(false);
	
	Poll p(log4cxx::Logger::getRootLogger(), &sender);
	
   	zmq::message_t message(2);
 	msgpack::type::tuple<int, int, int> wmsg(0, 1, 2);
 	msgpack::sbuffer sbuf;
   	msgpack::pack(sbuf, wmsg);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    
	BOOST_CHECK(p.send(&w, message, 0, 0));
	
}

BOOST_AUTO_TEST_SUITE_END()
