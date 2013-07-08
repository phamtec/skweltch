
#ifndef __WORDSPLITTER_HPP_INCLUDED__
#define __WORDSPLITTER_HPP_INCLUDED__

#include <string>
#include <iostream>

class IWord {

public:
	virtual void word(const std::string &s) = 0;
};

class WordSplitter {

private:
	std::istream *input;
	
public:
	WordSplitter(std::istream *i) : input(i) {}
	
	void process(IWord *processor);
	
};

#endif // __WORDSPLITTER_HPP_INCLUDED__

