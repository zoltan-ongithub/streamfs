//
// Created by Zoltan Kuscsik on 4/26/20.
//

#ifndef STREAMLINK_PLUGINCBIMPL_H
#define STREAMLINK_PLUGINCBIMPL_H

#include <vector>
#include <mutex>
#include "PluginConfig.h"
#include "PluginCallbackInterface.h"

namespace streamfs {

class PluginCbImpl : public PluginCallbackInterface {
public:
    void setAvailableStreams(std::vector<std::string> streamIds) override;
    void updateConfig(PluginConfig &config) override;
    std::vector<std::string> getAvailableStreams() override;
private:
    std::mutex guard;
    std::vector<std::string> mStreamIds;
};

}
#endif //STREAMLINK_PLUGINCBIMPL_H
