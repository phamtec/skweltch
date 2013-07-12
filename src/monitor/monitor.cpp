
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "ExeRunner.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"
#include "PipeBuilder.hpp"

#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "monitor.log", log::keywords::auto_flush = true);
	
	if (argc != 2) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " jsonConfig";
		return 1;
	}
	
	ifstream jsonfile(argv[1]);
	JsonConfig c(&jsonfile);
	JsonObject r;
	if (!c.read(&r)) {
		return 1;
	}
    
   	string pidFilename = r.getString("pidFile");
   	
   	ExeRunner er;

	// first stop anything.
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(pidFilename);

	// now run up the workers.
	try {
		JsonArray bg = r.getArray("background");
		ofstream pidfile(pidFilename.c_str());
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
		
			int count = bg.getInt(i, "count");
			string name = bg.getString(i, "name");
			string exe(bg.getString(i, "exe"));
			
			// build pipes for this node.
			JsonObject pipesjson = PipeBuilder().collect(&r, bg.getValue(i));
			stringstream pipes;
			pipesjson.write(false, &pipes);
			
			stringstream config;
			config << "'" << bg.getChildAsString(i, "config") << "'";
			if (count > 0) {
				for (int i=0; i<count; i++) {
					stringstream cmd;
					cmd << exe << " " << i << " '" << pipes.str() << "' " << config.str() << " " << name << "[" << i << "]";
					pid_t pid = er.run(cmd.str());
					pidfile << pid << endl;
				}
			}
			else {
				stringstream cmd;
				cmd << exe << " '" << pipes.str() << "' " << config.str() << " " << name;
				pid_t pid = er.run(cmd.str());
				pidfile << pid << endl;
			}
		}
	}
	catch (runtime_error &e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		return 1;
	}
	
	// something to vent.
	{
		JsonObject vent = r.getChild("vent");
		
		// build pipes for this node.
		JsonObject pipesjson = PipeBuilder().collect(&r, vent);
		stringstream pipes;
		pipesjson.write(false, &pipes);

		stringstream exe;
		exe << vent.getString("exe") << " '" << pipes.str() << "' '" << vent.getChildAsString("config") << "' " << vent.getString("name");
		er.run(exe.str());
	}
	
	// something to reap.
	{
		JsonObject reap = r.getChild("reap");
		
		// build pipes for this node.
		JsonObject pipesjson = PipeBuilder().collect(&r, reap);
		stringstream pipes;
		pipesjson.write(false, &pipes);

		stringstream exe;
		exe << reap.getString("exe") << " '" << pipes.str() << "' '" << reap.getChildAsString("config") << "' " << reap.getString("name");
		er.run(exe.str());
	}

	return 0;
	
}
