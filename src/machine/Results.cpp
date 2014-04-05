
#include "Results.hpp"

#include <iostream>

using namespace std;

void StreamResults::write(int group, int iter, const string &vars, int iterations, int low, int high, int fail, int sum) {

	double avg = (double)sum / (double)iterations;
	double med = low + ((double)(high - low) / 2.0);
    
	*stream << group << "\t" << iter << "\t" << vars << "\t" << iterations << "\t" << low << "\t" << high <<
        "\t" << fail << "\t" << avg << "\t" << med << endl;
    

}
