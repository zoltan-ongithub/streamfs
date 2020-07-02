//
// Created by Zoltan Kuscsik on 4/26/20.
//

#include "streamfs/PluginCbImpl.h"

namespace streamfs {

void PluginCbImpl::setAvailableStreams(std::vector<std::string> streamIds)
{
    std::lock_guard<std::mutex> lock(guard);
    mStreamIds = streamIds;

}

void PluginCbImpl::updateConfig(PluginConfig &config) {
    std::lock_guard<std::mutex> lock(guard);

}

std::vector<std::string> PluginCbImpl::getAvailableStreams() {
    std::lock_guard<std::mutex> lock(guard);
    return mStreamIds;
}

}