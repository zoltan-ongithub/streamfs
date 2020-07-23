#pragma once
#include <vector>
#include <string>
#include "PluginConfig.h"
namespace streamfs {
class PluginCallbackInterface {
public:
    virtual void setAvailableStreams(std::vector<std::string> streamIds) = 0;

    virtual void updateConfig(PluginConfig &config) = 0;

    virtual std::vector<std::string>  getAvailableStreams() = 0;

    /**
     * Notify file changed (used in poll)
     * @param path
     */
    virtual void notifyUpdate(std::string path) = 0;

    virtual ~PluginCallbackInterface() = default;
};
}