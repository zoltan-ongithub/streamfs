//
// Created by Zoltan Kuscsik on 4/17/20.
//


#include "streamfs/StreamPluginManager.h"
#include "streamfs/PluginCbImpl.h"
#include "streamfs/PluginCallbackInterface.h"
#include <boost/filesystem.hpp>
#include <set>
#include <dlfcn.h>
#include <glog/logging.h>
#include <streamfs/PluginInterface.h>
#include <streamfs/VirtualFSProvider.h>
#include <fuse/IFuse.h>

namespace fs = boost::filesystem;

namespace  streamfs {

StreamPluginManager::StreamPluginManager() = default;

int StreamPluginManager::loadPlugins(const PluginManagerConfig& configuration) {
    std::lock_guard<std::mutex> lock(mPluginMtx);
    std::set<std::string> sharedLibs;

    mPlugins.clear();

    for(auto dir: configuration.pluginDirectories) {
        if (!fs::exists(dir)) {
            continue;
        }
        for (const auto &entry : fs::directory_iterator(dir)) {
            if (fs::is_regular_file(entry.status())) {
                if (entry.path().extension() == ".so") {
                    sharedLibs.insert(entry.path().string());
                }
            }
        }
    }
    for (const std::string& soFile : sharedLibs) {
        LOG(INFO) << "Loading plugin:" << soFile << "\n";

        void *hndl = dlopen(soFile.c_str(), RTLD_NOW);

        if(hndl == nullptr){
            LOG(INFO) << "Can't open shared library: " << soFile << " Ignoring plugin.";
            LOG(INFO) << "   dlerror: " <<  dlerror();
            continue;
        }

        void* pluginLoad =  (void*) dlsym(hndl, "INIT_STREAMFS_PLUGIN");


        if (pluginLoad == nullptr) {
            LOG(INFO) << "Ignoring .so file " << soFile << ".  Missing INIT_STREAMFS_PLUGIN implementation.";
            LOG(INFO) << " dlerror:" << dlerror();
            dlclose(hndl);
            continue;
        }

       void* getPluginIdF =  (void*) dlsym(hndl, "GET_STREAMFS_PLUGIN_ID");

        if (getPluginIdF == nullptr) {
            LOG(INFO) << "Ignoring .so file " << soFile << ".  Missing GET_STREAMFS_PLUGIN_ID implementation.";
            LOG(INFO) << " dlerror:" << dlerror();
            dlclose(hndl);
            continue;
        }

        create_fn creator = nullptr;
        *reinterpret_cast<void**>(&creator) = pluginLoad;

        get_id_fn get_id_call = nullptr;

        *reinterpret_cast<void**>(&get_id_call) = getPluginIdF;

        const char* pluginIdTmp = get_id_call();

        if (pluginIdTmp == nullptr || mPlugins.find(pluginIdTmp) != mPlugins.end()) {

                LOG(WARNING) << "Ignoring duplicate plugin. Plugin " << pluginIdTmp << " path: " << soFile <<
                             " already loaded";
                dlclose(hndl);
                continue;
        }

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
        pluginState->libraryPath = soFile;

        if(plugin->getInterfaceVersion() != STREAMFS_INTERFACE_VERSION) {
            LOG(WARNING) << "Incorrect interface version: " << plugin->getInterfaceVersion() <<
                " minimum interface version requested: " << STREAMFS_INTERFACE_VERSION;
            LOG(WARNING) << "Plugin ignored: " << plugin->getId();
            continue;
        }

        mPlugins.insert(std::make_pair(std::string(plugin->getId()), pluginState));

        LOG(INFO) << "Loaded plugin:" << plugin->getId();
    }

    return 0;
}

void StreamPluginManager::initPlugins() {
    std::lock_guard<std::mutex> lock(mPluginMtx);
}

};

