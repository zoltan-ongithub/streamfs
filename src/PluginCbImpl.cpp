//
// Created by Zoltan Kuscsik on 4/26/20.
//

#include "streamfs/PluginCbImpl.h"

namespace streamfs {

void PluginCbImpl::setAvailableStreams(const std::vector<std::string> &streamIds) {
    std::lock_guard<std::mutex> lock(guard);
    mStreamIds = std::vector<std::string>(streamIds);
}

void PluginCbImpl::updateConfig(const PluginConfig &config) {
    std::lock_guard<std::mutex> lock(guard);

}

const std::vector<std::string> &PluginCbImpl::getAvailableStreams() {
    std::lock_guard<std::mutex> lock(guard);
    return mStreamIds;
}

void PluginCbImpl::notifyUpdate(const std::string &path) {
    std::lock_guard<std::mutex> lock(guard);
    mCBHandler->notifyUpdate(mId, path);
}

}