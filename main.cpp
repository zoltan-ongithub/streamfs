#include <iostream>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include "StreamPluginManager.h"
using namespace streamfs;
#include <glog/logging.h>
#include <fuse/IFuse.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

PluginManagerConfig config = {
        .pluginDirectories = {".", "/usr/lib/streamfs/", "/usr/local/lib/streamfs"}
};

StreamPluginManager pluginManager;
int main(int argc, char *argv[])
{
    assert(pluginManager.loadPlugins(config) == 0);
    pluginManager.initPlugins();
    fuse_operations o  = IFuse::getInstance().getFuseOperations();
    return fuse_main(argc, argv, &o, NULL);
}

