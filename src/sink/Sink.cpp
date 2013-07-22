
#include "Sink.hpp"

#include "ISinkWorker.hpp"
#include "Elapsed.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

bool Sink::process(ISinkWorker *worker) {

	Elapsed elapsed;
	int first = 0;
	int last = 0;
 	while (!worker->shouldQuit()) {
	
		zmq::message_t message;
 		try {
       		receiver->recv(&message);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger, "interrupt received, killing server")
			break;
		}

		msgpack::unpacked msg;
		msgpack::unpack(&msg, (const char *)message.data(), message.size());
		msgpack::object obj = msg.get();
		msgpack::type::tuple<int, int, int> sinkmsg;
		obj.convert(&sinkmsg);

		switch (sinkmsg.a0) {
		case 1:
			// this one usually comes from a vent.
			first = sinkmsg.a1;
			worker->first(first);
			LOG4CXX_INFO(logger, "First: " << first)
			elapsed.start();
			break;
			
		case 2:
			if (first == 0) {
  				LOG4CXX_ERROR(logger, "Don't know the first message yet." << obj)
   				return 1;
			}
			worker->process(sinkmsg.a1, sinkmsg.a2);

 			if (last > 0 && sinkmsg.a1 == last) {
			
  				LOG4CXX_INFO(logger, "Finished.")

				 //  Calculate and report duration of batch
				int total_msec = elapsed.getTotal();

				// get results.
				worker->results(total_msec);
   				LOG4CXX_INFO(logger, "Results written.")
   				
   				// start again
   				first = 0;
   				last = 0;
			}
			break;
			
		case 3:
			// this one usually comes from a vent.
			last = sinkmsg.a1;
			worker->last(last);
			LOG4CXX_INFO(logger, "Last: " << last)
			break;
			
		default:
  			LOG4CXX_ERROR(logger, "Unknown message: " << sinkmsg.a0)
   			return 1;
		}
	}
	
   	LOG4CXX_INFO(logger, "finished.")
	return true;
	
}

