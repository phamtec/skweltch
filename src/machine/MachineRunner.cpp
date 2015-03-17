
#include "MachineRunner.hpp"

#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"
#include "Interrupt.hpp"
#include "Results.hpp"
#include "StringMsg.hpp"
#include "KillMsg.hpp"
#include "Ports.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <czmq.h>
#include <zmq.hpp>

using namespace std;
using namespace boost;

bool MachineRunner::runOne(const string &machine, const string &resaddr, int control, int iterations, int group, int iter, const string &vars, const JsonObject &success) {

	ifstream jsonfile(machine.c_str());
	JsonObject r;
	if (!r.read(logger, &jsonfile)) {
		return false;
	}

	TaskMonitor mon(logger, control);
	int fail = 0;
	int low = -1;
	int high = -1;
	int sum = 0;

    zmq::context_t context(1);
    zmq::socket_t rsocket(context, ZMQ_PULL);
    rsocket.connect(resaddr.c_str());
    
    // create a control socket.
    zmq::socket_t csocket(context, ZMQ_PUB);
	if (!Ports::bind(&logger, &csocket, "*", control, "control")) {
		return false;
	}

	// startup the machine.
	vector<int> pids;
	if (!mon.start(&r, &pids)) {
		throw new runtime_error("couldn't run the machine.");
	}
	
    // write the pids out so tools can kill if something goes wrong.
    {
        ofstream pidfile("task.pids");
        for (vector<int>::iterator i=pids.begin(); i != pids.end(); i++) {
            pidfile << *i << endl;
        }
    }
    
    // ok we need to pause here for a little bit to make sure all of the workers have started before
    // we start venting. We should really use messaging for this...
    LOG4CXX_INFO(logger, "settling...")
    zclock_sleep(settleTime);
    
    // ok now we go for it.
	for (int i=0; !s_interrupted && i<iterations; i++) {
	
 		LOG4CXX_INFO(logger, "run " << i << " [")
 		
        // do the vent
		{
 			LOG4CXX_INFO(logger, "venting.")
			vector<int> vpids;
			if (!mon.doVent(&r, &vpids)) {
				return false;
			}
			// wait till vent is done.
			mon.waitFinish(vpids);
 			LOG4CXX_INFO(logger, "vent done.")
		}
			
        // and wait for the message to come.
		zmq::message_t message;
		try {
			rsocket.recv(&message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "recv failed." << e.what())
		}
    
		if (*interrupted) {
			break;
		}
		
 		JsonObject results;
        {
            StringMsg rmsg(message);
            stringstream ss(rmsg.getPayload());
            results.read(logger, &ss);
        }
        
		if (!results.empty()) {
		
			// compare the results against the success condition.
			bool bad = false;
			if (!success.empty()) {
				JsonObject &o = const_cast<JsonObject &>(success);
				for (JsonObject::iterator i=o.begin(); !bad && i != o.end(); i++) {
					string k = o.getKey(i);
					if (o.isValueDouble(i)) {
						double v = o.getValueDouble(i);
						double rv = results.getDouble(k);
						if (abs(v - rv) > 0.005) {
							LOG4CXX_ERROR(logger, "failure because " << k << "=" << rv << " not " << v)
							bad = true;
						}
					}
					else {
						string v = o.getValueString(i);
						string rv = results.getAsString(k);
						if (v != rv) {
							LOG4CXX_ERROR(logger, "failure because " << k << "=" << rv << " not " << v)
							bad = true;
						}
					}
				}
			}
			if (bad) {
				fail++;
				if (failonerror) {
					break;
				}
			}
			else {
				int elapsed = results.getInt("elapsed", -1);
				if (low < 0 || elapsed < low) {
					low = elapsed;
				}
				if (high < 0 || elapsed > high) {
					high = elapsed;
				}
				sum += elapsed;
			}
		}
		else {
			LOG4CXX_ERROR(logger, "failure because no results.")
			fail++;
			if (failonerror) {
				break;
			}
		}
		LOG4CXX_INFO(logger, "] " << i)
	}
	
	// send a kill message on the control socket.
 	{
 		KillMsg msg;
		zmq::message_t message;
 		msg.set(&message);
		try {
			csocket.send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "control send failed." << e.what())
			return false;
		}
	}
		
	// wait till everything is gone.
	mon.waitFinish(pids);

	results->write(group, iter, vars, iterations, low, high, fail, sum);

	return fail == 0;

}
