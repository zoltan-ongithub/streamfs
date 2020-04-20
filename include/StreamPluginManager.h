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

public:
    /**
     * Dynamically load plugins.
     * This will also deallocate previously opened plugins
     * @param configuration
     * @return 0 on success
     */
    int loadPlugins(PluginManagerConfig configuration);

    explicit StreamPluginManager();

private:
    std::map<std::string, std::unique_ptr<streamfs::PluginInterface>> mPlugins;
};
}


#endif //STREAMFS_STREAMPLUGINMANAGER_H
