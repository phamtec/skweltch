#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../Sink.hpp"
#include "../SinkMsg.hpp"
#include "../ISinkWorker.hpp"
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

BOOST_AUTO_TEST_SUITE( sinkTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_sink_worker, ISinkWorker )
{
	MOCK_METHOD( first, 1 )
	MOCK_METHOD( process, 2 )
	MOCK_METHOD( last, 1 )
	MOCK_METHOD( results, 1 )
	MOCK_METHOD( shouldQuit, 0 )
	
};

MOCK_BASE_CLASS( mock_poller, IPoller )
{
	MOCK_METHOD( poll, 2 )
};

class MockReceiver: public zmq::i_socket_t {

private:
	int state;
	int id;
	int first;
	int last;
	int first2;
	int last2;
	vector<string> data;
	
public:
	MockReceiver(int f1, int l1, int f2, int l2, const vector<string> &d) : state(0), id(1), first(f1), last(l1), first2(f2), last2(l2), data(d) {}
	
	virtual bool send (zmq::message_t &msg_, int flags_ = 0) { return false; }
	
	virtual bool recv (zmq::message_t *msg_, int flags_ = 0)  {
		
		SinkMsg sinkmsg;

		switch (state) {
		case 0:
			sinkmsg.firstMsg(first);
			id = first;
			break;
			
		case 1:
			sinkmsg.lastMsg(last);
			break;
		
		default:
			sinkmsg.dataMsg(id++, data);
		}
		
		sinkmsg.set(msg_);

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
	mock_sink_worker w;
	mock_poller p;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.last).with(20).once();
	MOCK_EXPECT(w.process).with(20, mock::any).once();
	MOCK_EXPECT(w.results).with(mock::any).once();
	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfterTimes, &c, 20, 20));
	MOCK_EXPECT(p.poll).with(mock::any, 2000).returns(true);

	vector<string> v;
	v.push_back("100");
	MockReceiver receiver(20, 20, -1, -1, v);
	
	Sink s(log4cxx::Logger::getRootLogger(), &p, &receiver);
	BOOST_CHECK(s.process(&w));

}


BOOST_AUTO_TEST_CASE( tenMsgTest )
{
	mock_sink_worker w;
	mock_poller p;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.last).with(29).once();
	for (int i=20; i<=29; i++) {
		MOCK_EXPECT(w.process).with(i, mock::any).once();
	}
	MOCK_EXPECT(w.results).with(mock::any).once();

	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfterTimes, &c, 20, 29));
	MOCK_EXPECT(p.poll).with(mock::any, 2000).returns(true);

	vector<string> v;
	v.push_back("100");
	MockReceiver receiver(20, 29, -1, -1, v);
	
	Sink s(log4cxx::Logger::getRootLogger(), &p, &receiver);
	BOOST_CHECK(s.process(&w));

}

int shouldQuitAfter2Times( int *i, int first, int last )
{
    return (*i)++ > ((last-first+1)*2 + 3*2 + 1);
}

BOOST_AUTO_TEST_CASE( restartTest )
{
	mock_sink_worker w;
	mock_poller p;
	
	MOCK_EXPECT(w.first).with(20).once();
	MOCK_EXPECT(w.first).with(24).once();
	MOCK_EXPECT(w.last).with(23).once();
	MOCK_EXPECT(w.last).with(26).once();
	for (int i=20; i<=26; i++) {
		MOCK_EXPECT(w.process).with(i, mock::any).once();
	}
	MOCK_EXPECT(w.results).with(mock::any).exactly(2);
	MOCK_EXPECT(p.poll).with(mock::any, 2000).returns(true);

	int c=0;
	MOCK_EXPECT(w.shouldQuit).calls(boost::bind(&shouldQuitAfter2Times, &c, 20, 26));

	vector<string> v;
	v.push_back("100");
	MockReceiver receiver(20, 23, 24, 26, v);
	
	Sink s(log4cxx::Logger::getRootLogger(), &p, &receiver);
	BOOST_CHECK(s.process(&w));

}

BOOST_AUTO_TEST_SUITE_END()
