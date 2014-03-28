
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "IPollWorker.hpp"
#include "Poll.hpp"
#include "StringMsg.hpp"
#include "FileModChecker.hpp"
#include "Logging.hpp"
#include "Main.hpp"

#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>
#include <git2/clone.h>
#include <git2/config.h>
#include <git2/remote.h>
#include <git2/merge.h>
#include <git2/commit.h>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
using namespace boost::filesystem;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.pollgit"));

class PWorker : public IPollWorker {

private:
	string dir;
	long lastCrc;
	int sleeptime;
	
public:
	PWorker(const string &d, int crc, int sl) : dir(d), lastCrc(crc), sleeptime(sl) {}
	
	virtual bool waitEvent();
	virtual int send(int msgid, Poll *poll);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
	
};

bool PWorker::waitEvent() {

	// what's the crc
	long crc = FileModChecker(logger).getCrc(dir);
	while (crc == lastCrc) {
	
		// sleep for a bit.
		zclock_sleep(sleeptime);	
		
		// and try for the crc again.
		crc = FileModChecker(logger).getCrc(dir);
	}
	lastCrc = crc;
	
	return true;
}

int PWorker::send(int msgid, Poll *poll) {

	zmq::message_t message(2);
	StringMsg msg(msgid, clock(), dir);
	msg.set(&message);
	
	poll->send(this, message, 0, 0);

	return msgid+1;
	
}

int last_pc = 0;

static int fetch_progress(const git_transfer_progress *stats, void *payload)
{
    int fetch_percent = (100 * stats->received_objects) / stats->total_objects;
    
    if (fetch_percent != last_pc) {
        int kbytes = stats->received_bytes / 1024;
        cout << fetch_percent << "% (" << kbytes << " kb, " << stats->received_objects << "/" << stats->total_objects << ")" << endl;
        last_pc = fetch_percent;
    }
 
    
    return 0;
}

bool any_changes = false;
static int progress_cb(const char *str, int len, void *data)
{
    any_changes = true;
	return 0;
}

/**
 
 Example args:
 
 "{'pushTo':{'mode':'bind','address':'*','port':5557}}"
 
 "{'dir':'/Users/paul/Documents/test','sleeptime':500}"

 "test"
 
 Then test with:
 
 ./listen connect tcp://localhost:5557
 
*/

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    
    string folder = "/Users/paul/Documents/gittest";
    string repos = "git://github.com/phamtec/skweltch.git";
    
    git_repository *repo = NULL;
    
    if (exists(folder)) {
        
        // get the remote path
        int ret = git_repository_open(&repo, folder.c_str());
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_repository_open error " << ret << ".");
            return ret;
        }
        git_config *cfg = NULL;
        ret = git_repository_config(&cfg, repo);
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_repository_config error " << ret << ".");
            return ret;
        }
        const char *remoteurl;
        ret = git_config_get_string(&remoteurl, cfg, "remote.origin.url");
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_config_get_string error " << ret << ".");
            return ret;
        }
        git_config_free(cfg);
 
        if (repos != remoteurl) {
            LOG4CXX_ERROR(logger, "repos points to a different remote url.");
            return ret;
        }
        git_repository_free(repo);
 
        // ok we use the exerunner to pull because our attempt at using the api sucks.
        filesystem::current_path(folder);
        
        FILE *in;
        if (!(in = popen("git pull", "r"))){
            LOG4CXX_ERROR(logger, "popen failed.");
            return 1;
        }
        char buff[512];
        while (fgets(buff, sizeof(buff), in) != NULL) {
            cout << ": " << buff;
        }
        pclose(in);

    }
    else {
        // clone with git.
        git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
        opts.remote_callbacks.transfer_progress = fetch_progress;
        int ret = git_clone(&repo, repos.c_str(), folder.c_str(), &opts);
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_clone error " << ret << ".");
        }
        git_repository_free(repo);
    }
/*
	// the directory to watch.
	string dir = root.getString("dir");
	int sleeptime = root.getInt("sleeptime", 1000);

	// get the initial crc (we wait until this changes).
	long crc = FileModChecker(logger).getCrc(dir);
    
    //  Send count tasks
    s_catch_signals ();
    
	// and do the vent.
	Poll p(logger, &sender);
	PWorker w(dir, crc, sleeptime);
	if (p.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}
*/
}

int pull(git_repository *repo) {
    
    git_remote *remote;
    int ret = git_remote_load(&remote, repo, "origin");
    if (ret != 0) {
        LOG4CXX_ERROR(logger, "git_remote_load error " << ret << ".");
        return ret;
    }
    git_remote_callbacks remopts = GIT_REMOTE_CALLBACKS_INIT;
    remopts.progress = progress_cb;
    ret = git_remote_set_callbacks(remote, &remopts);
    if (ret != 0) {
        LOG4CXX_ERROR(logger, "git_remote_init_callbacks error " << ret << ".");
        return ret;
    }
    ret = git_remote_connect(remote, GIT_DIRECTION_FETCH);
    if (ret != 0) {
        LOG4CXX_ERROR(logger, "git_remote_connect error " << ret << ".");
        return ret;
    }
    ret = git_remote_fetch(remote, NULL, NULL);
    if (ret != 0) {
        LOG4CXX_ERROR(logger, "git_remote_fetch error " << ret << ".");
        return ret;
    }
    git_remote_free(remote);
    
    if (any_changes) {
        LOG4CXX_INFO(logger, "There were changes.");
        
        // merge the changes locally.
        git_reference *headref = NULL;
        ret = git_reference_lookup(&headref, repo, "FETCH_HEAD");
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_reference_lookup error " << ret << ".");
            return ret;
        }
        git_merge_head *head = NULL;
        ret = git_merge_head_from_ref(&head, repo, headref);
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_merge_head_from_ref error " << ret << ".");
            return ret;
        }
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
        checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
        git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
        ret = git_merge(repo, (const git_merge_head **)&head, 1, &merge_opts, &checkout_opts);
        if (ret != 0) {
            LOG4CXX_ERROR(logger, "git_merge error " << ret << ".");
            return ret;
        }
        
        git_merge_head_free(head);

    }
    
    return 0;
}
