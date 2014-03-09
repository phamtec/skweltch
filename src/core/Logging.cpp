//
//  Logging.cpp
//  Skweltch
//
//  Created by Paul Hamilton on 2/03/2014.
//
//

#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
using namespace log4cxx;
using namespace log4cxx::helpers;

void setup_logging() {
    
    if (!filesystem::exists("log4cxx.conf")) {
        string log4cxx = "log4j.rootLogger=INFO, STDOUT\n\n"
        "log4j.appender.STDOUT=org.apache.log4j.ConsoleAppender\n"
        "log4j.appender.STDOUT.layout=org.apache.log4j.PatternLayout\n"
        "#log4j.appender.STDOUT.layout.ConversionPattern=%d %-5p %c - %m%n\n"
        "log4j.appender.STDOUT.layout.ConversionPattern=%m%n\n"
        "log4j.appender.STDOUT.threshold=INFO\n";
		ofstream f("log4cxx.conf");
        f.write(log4cxx.c_str(), log4cxx.length());
    }
    
	// Set up a simple configuration that logs on the console.
	PropertyConfigurator::configure("log4cxx.conf");
    
}
