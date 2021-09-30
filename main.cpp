#include <iostream>
#include <assert.h>

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include "streamfs/StreamPluginManager.h"
using namespace streamfs;

#include <glog/logging.h>
#include <fuse/IFuse.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <dlfcn.h>
#include <sys/mount.h>

PluginManagerConfig config = {
        .pluginDirectories = {".", "/usr/lib/streamfs/", "/usr/local/lib/streamfs"}
};


StreamPluginManager pluginManager;

class parsed_args {
public:
    std::map<std::string, std::string> reserved_args; //args not passed to fuse
    int argc = 0;
    char** argv;

    ~parsed_args() {
        delete argv;
    }
};

static int isSigtermTriggered = 0;
static pthread_mutex_t sighandler_mutex;
static char* fuseMountPoint = nullptr;

void static sigterm_handler(int sig_received) {
    if ((pthread_mutex_trylock(&sighandler_mutex) == 0)) {
      if (isSigtermTriggered == 0) {
        isSigtermTriggered = 1;

        pluginManager.unloadPlugins();

        /*clear all previous errors if any*/
        dlerror();

        if (fuseMountPoint != nullptr) {
          int ret = umount2(fuseMountPoint,MNT_DETACH);
          if (ret != 0)
            std::cerr << "umount2 fails : "<< strerror(errno) << std::endl;
        }
        else
          std::cerr << "invalid fuseMountPoint " << std::endl;

        signal(sig_received , SIG_DFL);
        raise(sig_received);
      }
      pthread_mutex_unlock(&sighandler_mutex);
    }
}
void extract_reserved_args(int argc, char* argv[], parsed_args& p, uint32_t& debug_level) {
    // TODO: make this more generic. Currently we only extract debug level info
    bool found_debug_arg = false;
    uint32_t debug_arg_loc = 0;
    for(int i = 0; i < argc; i++) {
        if ( strncmp(argv[i], "--debug_level", 13) == 0) {
            if (i < argc - 1) {
                found_debug_arg = true;
                debug_arg_loc = i;
            } else {
                std::cerr << "Error: --debug_level needs an argument" << std::endl;
                exit(-1);
            }
        }
    }

    if (!found_debug_arg) {
        p.argc = argc;
        p.argv = new char*[p.argc + 1];
        for(int i = 0; i < argc; i++) {
            p.argv[i] = argv[i];
        }
    } else {
        p.argc = argc - 2;
        p.argv = new char*[ p.argc + 1];
        int i = 0;
        int j = 0;
        do  {
            // skip
            if (debug_arg_loc == i) {
                try {
                    debug_level = std::stoul(argv[i + 1], nullptr, 16);
                } catch (...) {
                    debug_level  = atoi(argv[i + 1]);
                }
                i += 2;
                continue;
            }
            p.argv[j] = argv[i];
            j++;
            i++;
        } while (j < p.argc);
    }
}

int main(int argc, char *argv[])
{
    FLAGS_colorlogtostderr = true;
    FLAGS_logtostderr = 1;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;

    parsed_args p;
    uint32_t  debug_level;

    extract_reserved_args(argc, argv, p, debug_level);

    IFuse::mDebugLevel = debug_level;
    if (debug_level !=0 ) {
        LOG(INFO) << "Enable debug flags:" << debug_level << std::endl;
    }

    struct fuse_args args = FUSE_ARGS_INIT(p.argc, p.argv);

    if (fuse_parse_cmdline(&args, &fuseMountPoint, NULL, NULL) != 0) {
      std::cout << "fuse_parse_cmdline Failed " << std::endl;
    }

    //TODO: fuse signal handler need to be used in future
    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));
    sigAction.sa_handler = sigterm_handler;
    sigAction.sa_flags = SA_ONSTACK;
    sigaction (SIGTERM, &sigAction, NULL);

    google::InitGoogleLogging(p.argv[0]);
    assert(pluginManager.loadPlugins(config) == 0);
    pluginManager.initPlugins();
    fuse_operations o  = IFuse::getInstance().getFuseOperations();

    return fuse_main(p.argc, p.argv, &o, NULL);
}

