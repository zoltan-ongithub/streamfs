//
// Created by Zoltan Kuscsik
//

#ifndef STREAMLINK_PLUGINSERVICE_H
#define STREAMLINK_PLUGINSERVICE_H

#include <thread>
#include <MessageQueue.h>
#include <ServiceRequest.h>

namespace streamfs {
class PluginInterface;

class PluginService {
public:
    PluginService(PluginInterface *iFace) {}

    PluginService() : mThread(nullptr) {}

    void start();

private:
    void loop();

    bool pendingExit();

private:
    std::thread *mThread;
    PluginInterface *mIFace;
    MessageQueue<ServiceRequest> mServReq;
};

}//namespace streamfs
#endif //STREAMLINK_PLUGINSERVICE_H
