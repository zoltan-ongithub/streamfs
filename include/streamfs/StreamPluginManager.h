//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_STREAMPLUGINMANAGER_H
#define STREAMFS_STREAMPLUGINMANAGER_H
#include <memory>
#include <map>
#include <string>
#include "PluginManagerConfig.h"
#include "PluginImpl.h"
#include "streamfs/PluginInterface.h"

namespace streamfs {

/**
 * Plugin manager for dynamically loaded stream plugins
 */
class StreamPluginManager {
    /**
     * Plugin state includes the plugin interface the virtual FS
     * provided and the load path of the plugin.
     */
    struct PluginState {
        std::shared_ptr<streamfs::PluginInterface> interface;
        std::shared_ptr<VirtualFSProvider> provider;
        std::string libraryPath;
    };

public:
    /**
     * Dynamically load plugins.
     * This will also deallocate previously opened plugins
     * @param configuration
     * @return 0 on success
     */
    int loadPlugins(const PluginManagerConfig& configuration);

    explicit StreamPluginManager();

    /***
     * Initialise plugins
     */
    void initPlugins();

    /**
     * Return plugin state for a give plugin.
     *
     * @param pluginId
     * @return pointer to plugin state. Returns null pointer if plugin not loaded
     */
    std::shared_ptr<PluginState> getPluginState(std::string pluginId) {
        auto res = mPlugins.find(pluginId);

        if (res == mPlugins.end())
            return nullptr;

        return res->second;
    }

private:
    std::map<std::string, std::shared_ptr<PluginState>> mPlugins;
    std::mutex mPluginMtx;

};
}


#endif //STREAMFS_STREAMPLUGINMANAGER_H
