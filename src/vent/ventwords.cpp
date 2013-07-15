
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

class SendWord : public IWord {

private:
	zmq::socket_t *sender;
	int sleeptime;
	int sleepevery;
	int count;
	
public:
	SendWord(zmq::socket_t *s, int slp, int ev) : sender(s), sleeptime(slp), sleepevery(ev), count(0) {}
	
	virtual void word(const std::string &s);
};

void SendWord::word(const std::string &s) {

 	zmq::message_t message(2);
	
	// pack the number up and send it.
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, s);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
 
	sender->send(message);
	count++;
	
	if (sleeptime > 0) {
		if ((count % sleepevery) == 0) {
			zclock_sleep(sleeptime);	
		}
	}
}

int main (int argc, char *argv[])
{
 	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "log/vent.log", log::keywords::auto_flush = true);
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config name";
		return 1;
	}
	
	stringstream outfn;
	outfn << "log/" << argv[3] << ".log";
	log::add_file_log(log::keywords::file_name = outfn.str(), log::keywords::auto_flush = true);
	
	JsonObject pipes;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}

	string filename = root.getString("filename");
	
 	int sleeptime = root.getInt("sleep", 0);
	int sleepevery = root.getInt("every", 0);
	
	BOOST_LOG_TRIVIAL(debug) << "sleeping " << sleeptime << " every " << sleepevery;
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
	zmq::socket_t sink(context, ZMQ_PUSH);

 	Ports ports;
    ports.join(&sender, pipes, root, "pushTo");
    ports.join(&sink, pipes, root, "syncTo");

	// pack the number up and send it.
    zmq::message_t message(2);
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, 1);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    sink.send(message);

	ifstream f(filename.c_str(), ifstream::in);
	if (!f.is_open()) {
		BOOST_LOG_TRIVIAL(info) << " no file " << filename;
		return 1;
	}
	
	SendWord w(&sender, sleeptime, sleepevery);
	WordSplitter splitter(&f);
	splitter.process(&w);
    
	BOOST_LOG_TRIVIAL(info) << "finished.";

    return 0;
}
