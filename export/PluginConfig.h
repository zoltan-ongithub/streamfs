//
// Created by Zoltan Kuscsik
//

#ifndef STREAMLINK_PLUGINCONFIG_H
#define STREAMLINK_PLUGINCONFIG_H

#include <map>

class PluginConfig  {
public:
    explicit PluginConfig() = default;
    PluginConfig(PluginConfig const & other) = default;
public:
    // Generic configuration variables
    std::map<std::string, std::string> configVars;

    uint64_t seek_buffer_size_ms;
};

#endif //STREAMLINK_PLUGINCONFIG_H
