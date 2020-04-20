#include <iostream>
#include <assert.h>

#include "StreamPluginManager.h"
using namespace streamfs;
#include <glog/logging.h>

void TEST_loadPlugins() {

    PluginManagerConfig config = {
            .plugin_directory = "."
    };

    StreamPluginManager pluginManager;
    assert(pluginManager.loadPlugins(config) == 0);

}

int main(int argc, char* argv[]) {
    TEST_loadPlugins();
    return 0;
}