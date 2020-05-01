//
// Created by Zoltan Kuscsik on 4/17/20.
//


#include "StreamPluginManager.h"
#include "PluginCbImpl.h"
#include "PluginCallbackInterface.h"
#include <filesystem>
#include <iostream>
#include <set>
#include <dlfcn.h>
#include <glog/logging.h>
#include <PluginInterface.h>
#include <VirtualFSProvider.h>
#include <fuse/IFuse.h>

namespace fs = std::filesystem;

namespace  streamfs {

StreamPluginManager::StreamPluginManager() = default;

int StreamPluginManager::loadPlugins(const PluginManagerConfig& configuration) {
    std::lock_guard<std::mutex> lock(mPluginMtx);
    std::set<std::string> sharedLibs;

    mPlugins.clear();

    for (const auto & entry : fs::directory_iterator(configuration.plugin_directory)) {

        if(fs::is_regular_file(entry.status())) {
           if( entry.path().extension() == ".so") {
                sharedLibs.insert(entry.path());
           }
        }
    }

    for (const std::string& soFile : sharedLibs) {
        LOG(INFO) << "Loading plugin:" << soFile << "\n";

        void *hndl = dlopen(soFile.c_str(), RTLD_NOW);

        if(hndl == nullptr){
            LOG(INFO) << "Can't open shared library:" << soFile << ". Ignoring plugin.";
            continue;
        }

        void* pluginLoad =  (streamfs::PluginInterface*) dlsym(hndl, "INIT_STREAMFS_PLUGIN");

        if (pluginLoad == nullptr) {
            LOG(INFO) << "Ignoring .so file " << soFile << ".  Missing INIT_STREAMFS_PLUGIN implementation.";
            LOG(INFO) << " dlerror:" << dlerror();
            dlclose(hndl);
            continue;
        }

        typedef streamfs::PluginInterface*(*create_fn)(PluginCallbackInterface*);

        create_fn creator = nullptr;
        *reinterpret_cast<void**>(&creator) = pluginLoad;


        auto* pin(new streamfs::PluginCbImpl());
        auto* cb = dynamic_cast<PluginCallbackInterface*>(pin);

        std::shared_ptr<PluginState> pluginState(new PluginState());

        std::shared_ptr<streamfs::PluginInterface>  plugin(creator(cb));

        if (plugin == nullptr) {
            dlclose(hndl);
            LOG(INFO) << "Can't load shared library:" << soFile << ".  INIT_STREAMFS_PLUGIN() failed.";
            continue;
        }

        pluginState->interface = plugin;
        auto& fsProvider = IFuse::getInstance();

        std::shared_ptr<VirtualFSProvider> provider(new VirtualFSProvider(plugin->getId(),
                plugin, fsProvider, true));

        pluginState->interface =  std::shared_ptr<streamfs::PluginInterface>(plugin);

        pluginState->provider = provider;

        mPlugins.insert(std::make_pair(std::string(plugin->getId()), pluginState));

        LOG(INFO) << "Loaded plugin:" << plugin->getId();
    }

    return 0;
}

void StreamPluginManager::initPlugins() {
    std::lock_guard<std::mutex> lock(mPluginMtx);

}
};

