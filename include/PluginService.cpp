//
// Created by Zoltan Kuscsik on 4/23/20.
//

#include <thread>
#include <glog/logging.h>
#include "PluginService.h"
#include "PluginInterface.h"
namespace  streamfs {

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
                mIFace->startPlayback(msg.uri);
                break;
            case ServiceRequestEnum ::STOP:
                mIFace->stopPlayback();

            default:
                LOG(WARNING) << "Service message type " << msg.type << " not in use";
        }
    }
}

bool PluginService::pendingExit() {
    return false;
}

}//namespace streamfs;