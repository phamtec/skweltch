
#include "Git.hpp"

#include <boost/filesystem.hpp>
#include <git2/clone.h>
#include <git2/config.h>
#include <git2/remote.h>
#include <git2/merge.h>
#include <git2/commit.h>

using namespace std;
using namespace boost;
using namespace boost::filesystem;

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

bool Git::shellPull(const string &folder) {
    
    filesystem::current_path(folder);
    
    FILE *in;
    if (!(in = popen("git pull", "r"))){
        LOG4CXX_ERROR(logger, "popen failed.");
        return 1;
    }
    
    char buff[512];
    stringstream ss;
    while (fgets(buff, sizeof(buff), in) != NULL) {
        ss << buff;
    }
    pclose(in);
    
    return ss.str() != "Already up-to-date.\n";
    
}

bool Git::poll(const std::string &folder, const std::string &repos) {
    
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
        
        return shellPull(folder);
        
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
        
        return true;
    }

}

bool any_changes = false;
static int progress_cb(const char *str, int len, void *data)
{
    any_changes = true;
	return 0;
}

int Git::nativePull(git_repository *repo) {
    
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
