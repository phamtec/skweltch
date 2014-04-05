
#ifndef __RESULTS_HPP_INCLUDED__
#define __RESULTS_HPP_INCLUDED__

#include <string>
#include <ostream>

/*
	Results of a run.
*/

class Results {

public:

    virtual void write(int group, int iter, const std::string &vars, int iterations, int low, int high, int fail, int sum) = 0;
    
};

class StreamResults : public Results {
    
private:
    std::ostream *stream;
    
public:
    StreamResults(std::ostream *s) : stream(s) {}
    
    virtual void write(int group, int iter, const std::string &vars, int iterations, int low, int high, int fail, int sum);
};

#endif // __RESULTS_HPP_INCLUDED__

