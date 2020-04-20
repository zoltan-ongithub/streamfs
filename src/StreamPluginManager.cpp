//
// Created by Zoltan Kuscsik on 4/17/20.
//


#include "StreamPluginManager.h"
#include <filesystem>
#include <iostream>
#include <set>
#include <dlfcn.h>
#include <glog/logging.h>

namespace fs = std::filesystem;

namespace  streamfs {

StreamPluginManager::StreamPluginManager() {

}
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <PluginInterface.h>

int StreamPluginManager::loadPlugins(PluginManagerConfig configuration) {
    std::set<std::string> sharedLibs;

    mPlugins.clear();

    for (const auto & entry : fs::directory_iterator(configuration.plugin_directory)) {

        if(fs::is_regular_file(entry.status())) {
           if( entry.path().extension() == ".so") {
                sharedLibs.insert(entry.path());
           }
        }
    }

    for (std::string soFile : sharedLibs) {
        std::cout << "Loading plugins\n";

        void *hndl = dlopen(soFile.c_str(), RTLD_NOW);
        if(hndl == NULL){
            LOG(INFO) << "Can't open shared library:" << soFile << ". Ignoring plugin.";
            continue;
        }

        void* pluginLoad =  (streamfs::PluginInterface*) dlsym(hndl, "INIT_STREAMFS_PLUGIN");

        if (pluginLoad == NULL) {
            LOG(INFO) << "Unable to init plugin: " << soFile << ".  Missing INIT_STREAMFS_PLUGIN implementation.";
            dlclose(hndl);
        }

        typedef streamfs::PluginInterface*(*create_fn)();
        create_fn creator = 0;
        *reinterpret_cast<void**>(&creator) = pluginLoad;
        streamfs::PluginInterface*  plugin = creator();

        if (plugin == NULL) {
            dlclose(hndl);
            LOG(INFO) << "Can't load shared library:" << soFile << ".  INIT_STREAMFS_PLUGIN() failed.";
            continue;
        }

        mPlugins.insert(std::make_pair(std::string(plugin->getId()), std::unique_ptr<streamfs::PluginInterface>(plugin)));

        LOG(INFO) << "Loaded plugin:" << plugin->getId();
    }
    return 0;
}

};

