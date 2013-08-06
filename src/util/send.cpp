
#include <zmq.hpp>
#include <msgpack.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace log4cxx;
using namespace log4cxx::helpers;

void packval(msgpack::sbuffer *sbuf, const char *s);
void packpair(msgpack::sbuffer *sbuf, const char *s1, const char *s2);
void packtuple(msgpack::sbuffer *sbuf, const char *s1, const char *s2, const char *s3);

LoggerPtr logger(Logger::getLogger("org.skweltch.send"));

int main (int argc, char *argv[])
{
	// Set up a simple configuration that logs on the console.
	PropertyConfigurator::configure("log4cxx.conf");

	if (argc < 4) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " bind|connect port msg [msg msg ...]")
		LOG4CXX_ERROR(logger, "\tpacks a value, a pair, a tuple or a vector of strings and sends it.")
		return 1;
	}
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    
    if (string(argv[1]) == "bind") {
 		try {
			sender.bind(argv[2]);
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't bind should be: tcp://*:port")
			return 1;
		}
    }
    else if (string(argv[1]) == "connect") {
 		try {
			sender.connect(argv[2]);
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't connect should be: tcp://localhost:port")
			return 1;
		}
    }
    else {
 		LOG4CXX_ERROR(logger, "bind or connect. You choose.")
   	}
    	
   	zmq::message_t message(2);
	msgpack::sbuffer sbuf;
	switch (argc) {
	case 4:
		packval(&sbuf, argv[3]);
		break;
		
	case 5:
		packpair(&sbuf, argv[3], argv[4]);
		break;
		
	case 6:
		packtuple(&sbuf, argv[3], argv[4], argv[5]);
		break;
		
	default:
		{
			vector<string> v;
			for (int i=3; i<argc; i++) {
				v.push_back(argv[i]);
			}
			msgpack::pack(sbuf, v);		
		}
		break;
	}
	
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    sender.send(message);

	return 0;
	
}

void packval(msgpack::sbuffer *sbuf, const char *s) {

	try {
		msgpack::pack(*sbuf, boost::lexical_cast<int>(s));
	}
	catch (bad_lexical_cast &e) {
		msgpack::pack(*sbuf, string(s));		
	}

}

void packpair(msgpack::sbuffer *sbuf, const char *s1, const char *s2) {

	try {
		int v1 = boost::lexical_cast<int>(s1);
		try {
			msgpack::pack(sbuf, pair<int, int>(v1, boost::lexical_cast<int>(s2)));		
		}
		catch (bad_lexical_cast &e) {
			msgpack::pack(sbuf, pair<int, string>(v1, s2));		
		}
	}
	catch (bad_lexical_cast &e) {
		try {
			msgpack::pack(sbuf, pair<string, int>(s1, boost::lexical_cast<int>(s2)));		
		}
		catch (bad_lexical_cast &e) {
			msgpack::pack(sbuf, pair<string, string>(s1, s2));		
		}
	}

}

void packtuple(msgpack::sbuffer *sbuf, const char *s1, const char *s2, const char *s3) {

	try {
		int v1 = boost::lexical_cast<int>(s1);
		try {
			int v2 = boost::lexical_cast<int>(s2);
			try {
				msgpack::pack(sbuf, msgpack::type::tuple<int, int, int>(v1, v2, boost::lexical_cast<int>(s3)));		
			}
			catch (bad_lexical_cast &e) {
				msgpack::pack(sbuf, msgpack::type::tuple<int, int, string>(v1, v2, s3));		
			}
		}
		catch (bad_lexical_cast &e) {
			try {
				msgpack::pack(sbuf, msgpack::type::tuple<int, string, int>(v1, s2, boost::lexical_cast<int>(s3)));		
			}
			catch (bad_lexical_cast &e) {
				msgpack::pack(sbuf, msgpack::type::tuple<int, string, string>(v1, s2, s3));		
			}
		}
	}
	catch (bad_lexical_cast &e) {
		try {
			int v2 = boost::lexical_cast<int>(s2);
			try {
				msgpack::pack(sbuf, msgpack::type::tuple<string, int, int>(s1, v2, boost::lexical_cast<int>(s3)));		
			}
			catch (bad_lexical_cast &e) {
				msgpack::pack(sbuf, msgpack::type::tuple<string, int, string>(s1, v2, s3));		
			}
		}
		catch (bad_lexical_cast &e) {
			try {
				msgpack::pack(sbuf, msgpack::type::tuple<string, string, int>(s1, s2, boost::lexical_cast<int>(s3)));		
			}
			catch (bad_lexical_cast &e) {
				msgpack::pack(sbuf, msgpack::type::tuple<string, string, string>(s1, s2, s3));		
			}
		}
	}

}
