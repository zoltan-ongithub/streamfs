#pragma once
#include <vector>
#include <string>
#include "PluginConfig.h"
namespace streamfs {
class PluginCallbackInterface {
public:
    virtual void setAvailableStreams(const std::vector<std::string> &streamIds) = 0;

    virtual void updateConfig(const PluginConfig &config) = 0;

    virtual const std::vector<std::string> & getAvailableStreams() = 0;

    /**
     * Notify file changed (used in poll)
     * @param path
     */
    virtual void notifyUpdate(const std::string& path) = 0;

    virtual ~PluginCallbackInterface() = default;
};
}