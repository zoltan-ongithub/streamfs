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

debug_options_t IFuse::mDebugOptions;

// Custom command line options
static struct options {
    uint debug_level;
    bool ts_dump;
} options;

#define OPTION(t, p)                           \
     { t, offsetof(struct options, p), 1 }

static const struct fuse_opt option_spec[] = {
        OPTION("--debug_level=%d", debug_level),
        OPTION("--ts_dump=%d", ts_dump),
        FUSE_OPT_END
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

int main(int argc, char *argv[])
{
    FLAGS_colorlogtostderr = true;
    FLAGS_logtostderr = 1;
    FLAGS_logtostderr = true;
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return 1;

    IFuse::mDebugOptions.debugLevel = options.debug_level;
    IFuse::mDebugOptions.tsDumpEnable = options.ts_dump;

    if (options.debug_level !=0 ) {
        LOG(INFO) << "Enable debug flags:" << options.debug_level << std::endl;
    }

    if (options.ts_dump) {
        LOG(INFO) << "Enable ts dump:" << options.ts_dump << std::endl;
    }

    struct fuse_args args_cmd = FUSE_ARGS_INIT(argc, argv);
    if (fuse_parse_cmdline(&args_cmd, &fuseMountPoint, NULL, NULL) != 0) {
        std::cout << "fuse_parse_cmdline Failed " << std::endl;
    }
    fuse_opt_free_args(&args_cmd);

    //TODO: fuse signal handler need to be used in future
    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));
    sigAction.sa_handler = sigterm_handler;
    sigAction.sa_flags = SA_ONSTACK;
    sigaction (SIGTERM, &sigAction, NULL);

    google::InitGoogleLogging(argv[0]);
    assert(pluginManager.loadPlugins(config) == 0);
    pluginManager.initPlugins();

    fuse_operations o  = IFuse::getInstance().getFuseOperations();
    int ret = fuse_main(args.argc, args.argv, &o, NULL);

    fuse_opt_free_args(&args);

    return ret;
}
