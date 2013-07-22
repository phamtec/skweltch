#define BOOST_TEST_MODULE sink-tests
#include <boost/test/unit_test.hpp>

#include "../Sink.hpp"
#include "../ISinkWorker.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_sink_worker, ISinkWorker )
{
	MOCK_METHOD( first, 1 )
	MOCK_METHOD( process, 2 )
	MOCK_METHOD( last, 1 )
	MOCK_METHOD( results, 1 )
	MOCK_METHOD( shouldQuit, 0 )
	
};

class MockReceiver: public zmq::i_socket_t {

private:
	int state;
	int id;
	int first;
	int last;
	int first2;
	int last2;
	int data;
	
public:
	MockReceiver(int f1, int l1, int f2, int l2, int d) : state(0), id(1), first(f1), last(l1), first2(f2), last2(l2), data(d) {}
	
	virtual bool send (zmq::message_t &msg_, int flags_ = 0) { return false; }
	
	virtual bool recv (zmq::message_t *msg_, int flags_ = 0)  {
		
		msgpack::type::tuple<int, int, int> sinkmsg;
		switch (state) {
		case 0:
			sinkmsg.a0 = 1;
			sinkmsg.a1 = first;
			sinkmsg.a2 = 0;
			id = first;
			break;
			
		case 1:
			sinkmsg.a0 = 3;
			sinkmsg.a1 = last;
			sinkmsg.a2 = 0;
			break;
		
		default:
			sinkmsg.a0 = 2;
			sinkmsg.a1 = id++;
			sinkmsg.a2 = data;
		}
		
		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, sinkmsg);
		msg_->rebuild(sbuf.size());
		memcpy(msg_->data(), sbuf.data(), sbuf.size());

		state++;

		if (id > last) {
			state = 0;
			id = 0;
			if (first2 > 0 && last2 > 0) {
				first = first2;
				last = last2;
			}
		}
			
 		return true;
	}
	
	virtual void bind (const char *addr_) {}
	virtual void connect (const char *addr_) {}
	virtual void setsockopt (int option_, const void *optval_, size_t optvallen_) {}
	
};

int shouldQuitAfterTimes( int *i, int first, int last )
{
    return (*i)++ > ((last-first+1)*2 + 3);
}

BOOST_AUTO_TEST_CASE( oneMsgTest )
{
	log4cxx::BasicConfigurator::configure();

	mock_sink_worker w;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.last).with(20).once();
	MOCK_EXPECT(w.process).with(20, 100).once();
	MOCK_EXPECT(w.results).with(mock::any).once();
	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfterTimes, &c, 20, 20));

	MockReceiver receiver(20, 20, -1, -1, 100);
	
	Sink s(log4cxx::Logger::getRootLogger(), &receiver);
	BOOST_CHECK(s.process(&w));

}

BOOST_AUTO_TEST_CASE( tenMsgTest )
{
	log4cxx::BasicConfigurator::configure();

	mock_sink_worker w;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.last).with(29).once();
	for (int i=20; i<=29; i++) {
		MOCK_EXPECT(w.process).with(i, 100).once();
	}
	MOCK_EXPECT(w.results).with(mock::any).once();

	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfterTimes, &c, 20, 29));

	MockReceiver receiver(20, 29, -1, -1, 100);
	
	Sink s(log4cxx::Logger::getRootLogger(), &receiver);
	BOOST_CHECK(s.process(&w));

}

int shouldQuitAfter2Times( int *i, int first, int last )
{
    return (*i)++ > ((last-first+1)*2 + 3*2 + 1);
}

BOOST_AUTO_TEST_CASE( restartTest )
{
	log4cxx::BasicConfigurator::configure();

	mock_sink_worker w;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.first).with(24).once();
	MOCK_EXPECT(w.last).with(23).once();
	MOCK_EXPECT(w.last).with(26).once();
	for (int i=20; i<=26; i++) {
		MOCK_EXPECT(w.process).with(i, 100).once();
	}
	MOCK_EXPECT(w.results).with(mock::any).exactly(2);

	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfter2Times, &c, 20, 26));

	MockReceiver receiver(20, 23, 24, 26, 100);
	
	Sink s(log4cxx::Logger::getRootLogger(), &receiver);
	BOOST_CHECK(s.process(&w));

}
