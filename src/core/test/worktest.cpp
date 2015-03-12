#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../Work.hpp"
#include "../IWorkWorker.hpp"
#include "../Msg.hpp"
#include "../Poller.hpp"

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

BOOST_AUTO_TEST_SUITE( workTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( recv, 2 )
	MOCK_METHOD( bind, 1 )
	MOCK_METHOD( connect, 1 )
	MOCK_METHOD( setsockopt, 3 )
};

MOCK_BASE_CLASS( mock_work_worker, IWorkWorker )
{
	MOCK_METHOD( processMsg, 1 )
	MOCK_METHOD( shouldQuit, 0 )
};

MOCK_BASE_CLASS( mock_poller, IPoller )
{
	MOCK_METHOD( poll, 3 )
};

int shouldQuitAfterTimes( int *i, int times )
{
    return (*i)++ > times;
}

BOOST_AUTO_TEST_CASE( simpleTest )
{

	mock_socket receiver;
	mock_socket control;
	mock_work_worker w;
	mock_poller p;

	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfterTimes, &c, 2));
	MOCK_EXPECT(w.processMsg).with(mock::any).once();
	MOCK_EXPECT(receiver.recv).with(mock::any, 0).once().returns(true);
	MOCK_EXPECT(p.poll).with(mock::any, mock::any, 1000).once().returns(true);

	Work ww(log4cxx::Logger::getRootLogger(), &p, &receiver, &control);
	ww.process(&w);
	
}

BOOST_AUTO_TEST_SUITE_END()
