#include "Sink.hpp"
#include "JsonNode.hpp"
#include <iostream>
#include <fstream>
#include <sys/time.h>

using namespace std;
using namespace boost;

void Sink::service(JsonNode *root, zmq::i_socket_t *receiver) {

 	int expect = root->getInt("expect", 100);

    //  Wait for start of batch
    zmq::message_t message;
    receiver->recv(&message);

    //  Start our clock now
    struct timeval tstart;
    gettimeofday (&tstart, NULL);

    //  Process expected confirmations
    int task_nbr;
    int total_msec = 0;     //  Total calculated cost in msecs
    for (task_nbr = 0; task_nbr < expect; task_nbr++) {

    	receiver->recv(&message);
        if ((task_nbr / 10) * 10 == task_nbr)
            *outfile << ":" << std::flush;
        else
            *outfile << "." << std::flush;
    }
    
    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday (&tend, NULL);

    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    }
    else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;
    *outfile << "\nTotal elapsed time: " << total_msec << " msec\n" << std::endl;
    
}
