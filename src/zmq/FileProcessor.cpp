
#include "FileProcessor.hpp"
#include "IFileTask.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;
using namespace boost;

void FileProcessor::processFileIfExistsThenDelete(const std::string &filename)
{
	ifstream pidfile(filename.c_str());	
	if (pidfile.is_open()) {
		task->process(&pidfile);
		pidfile.close();
		remove(filename.c_str());
	}
}
