
#include "Elapsed.hpp"

Elapsed::Elapsed() {

    //  Start our clock now
    gettimeofday (&tstart, NULL);

}
	
int Elapsed::getTotal() {

    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday(&tend, NULL);
    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    }
    else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    return tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;
    
}
