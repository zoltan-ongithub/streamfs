//
// Created by Zoltan Kuscsik on 4/28/20.
//

#ifndef STREAMLINK_PLUGINCALLBACKINTERFACE_H
#define STREAMLINK_PLUGINCALLBACKINTERFACE_H

#include "PluginConfig.h"
namespace streamfs {
class PluginCallbackInterface {
public:
    virtual void setAvailableStreams(std::vector<std::string> streamIds) = 0;

    virtual void updateConfig(PluginConfig &config) = 0;

    virtual std::vector<std::string>  getAvailableStreams() = 0;

    virtual ~PluginCallbackInterface() = default;
};
}

#endif //STREAMLINK_PLUGINCALLBACKINTERFACE_H
