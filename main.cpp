#include <iostream>
#include <assert.h>

#include "StreamPluginManager.h"
using namespace streamfs;
#include <glog/logging.h>
#include <fuse/IFuse.h>

PluginManagerConfig config = {
        .plugin_directory = "."
};


StreamPluginManager pluginManager;

void TEST_loadPlugins(int argc, char* argv[]) {


    assert(pluginManager.loadPlugins(config) == 0);
    pluginManager.initPlugins();
    fuse_operations * o  = IFuse::getInstance().getFuseOperations();
    fuse_main(argc, argv, o, NULL);
}

int main(int argc, char* argv[]) {
    TEST_loadPlugins(argc, argv);
    return 0;
}