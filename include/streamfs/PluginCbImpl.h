//
// Created by Zoltan Kuscsik on 4/26/20.
//

#ifndef STREAMLINK_PLUGINCBIMPL_H
#define STREAMLINK_PLUGINCBIMPL_H

#include <vector>
#include <mutex>
#include "streamfs/PluginConfig.h"
#include "streamfs/PluginCallbackInterface.h"
#include <streamfs/VirtualFSProvider.h>

namespace streamfs {


class PluginCbImpl : public PluginCallbackInterface {
public:

    PluginCbImpl(std::string id) {
        mId = id;
    }

    void setAvailableStreams(const std::vector<std::string> &streamIds) override;
    void updateConfig(const PluginConfig &config) override;
    const std::vector<std::string> & getAvailableStreams() override;

    void notifyUpdate(const std::string &path) override;

    void registerCallbackHandler(VirtualFSProvider* fsProvider)  {
        mCBHandler = fsProvider;
    }

private:
    std::mutex guard;
    std::vector<std::string> mStreamIds;
    std::string mId;
    VirtualFSProvider *mCBHandler;
};

}
#endif //STREAMLINK_PLUGINCBIMPL_H
