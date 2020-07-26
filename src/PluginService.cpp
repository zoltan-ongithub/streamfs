//
// Created by Zoltan Kuscsik on 4/23/20.
//

#include <thread>
#include <glog/logging.h>
#include "streamfs/PluginService.h"

namespace streamfs {

void PluginService::start() {
    if (mThread != nullptr) {
        LOG(ERROR) << "Plugin service already started";
        return;
    }

    mThread = new std::thread(&PluginService::loop, this);
}

void PluginService::loop() {
    ServiceRequest msg;
    while (!pendingExit()) {
        mServReq.getMessage(msg);
        switch (msg.type) {
            case ServiceRequestEnum::START:
                break;
            case ServiceRequestEnum::STOP:
                stopPlayback();
            default:
                LOG(WARNING) << "Service message type " << msg.type << " not in use";
        }
    }
}

bool PluginService::pendingExit() {
    return false;
}

}//namespace streamfs;