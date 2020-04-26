//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_STREAMPLUGINMANAGER_H
#define STREAMFS_STREAMPLUGINMANAGER_H
#include <memory>
#include <map>
#include "PluginManagerConfig.h"
#include "PluginImpl.h"
#include "PluginInterface.h"

namespace streamfs {

/**
 * Plugin manager for dynamically loaded stream plugins
 */
class StreamPluginManager {
    struct PluginState {
        std::shared_ptr<streamfs::PluginInterface> interface;
        std::shared_ptr<PluginCallbackInterface> pluginCallbackInt;
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

    void initPlugins();

private:
    std::map<std::string, std::shared_ptr<PluginState>> mPlugins;
    std::mutex mPluginMtx;

};
}


#endif //STREAMFS_STREAMPLUGINMANAGER_H
