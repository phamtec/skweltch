#include "Reap.hpp"
#include "JsonConfig.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"
#include "ExeRunner.hpp"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <czmq.h>
#include <zclock.h>

using namespace std;
using namespace boost;

void Reap::service(JsonNode *root, zmq::i_socket_t *receiver) {
    
 	int totaltime = root->getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
 	*outfile << "waited " << totaltime << "ms, killing everything." << std::endl;
 	
   	ExeRunner er;
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(root->getString("pidFile"));

}
