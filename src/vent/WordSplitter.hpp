
#ifndef __WORDSPLITTER_HPP_INCLUDED__
#define __WORDSPLITTER_HPP_INCLUDED__

#include <string>
#include <iostream>

class IWord {

public:
	virtual bool word(const std::string &s) = 0;
};

class WordSplitter {

private:
	std::istream *input;
	
public:
	WordSplitter(std::istream *i) : input(i) {}
	
	bool process(IWord *processor);
	
};

#endif // __WORDSPLITTER_HPP_INCLUDED__

