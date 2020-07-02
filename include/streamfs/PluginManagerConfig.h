//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_PLUGINMANAGERCONFIG_H
#define STREAMFS_PLUGINMANAGERCONFIG_H

#include <string>
#include <vector>

/**
 * Configuration for plugin manager
 */
struct PluginManagerConfig {
    /**
     * Location of plugins
     */
    std::vector<std::string> pluginDirectories;
};


#endif //STREAMFS_PLUGINMANAGERCONFIG_H
