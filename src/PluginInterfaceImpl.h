//
// Created by Zoltan Kuscsik on 4/26/20.
//

#ifndef STREAMLINK_PLUGININTERFACEIMPL_H
#define STREAMLINK_PLUGININTERFACEIMPL_H

#include "PluginInterface.h"

namespace streamfs {
class PluginInterfaceImpl : public PluginCallbackInterface {
public:
    void setAvailableStreams(std::vector<std::string> streamIds) override;

    void updateConfig(PluginConfig &config) override;

};
}
#endif //STREAMLINK_PLUGININTERFACEIMPL_H
