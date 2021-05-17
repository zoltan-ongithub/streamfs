//
// Created by Zoltan Kuscsik
//

#ifndef STREAMLINK_PLUGINSERVICE_H
#define STREAMLINK_PLUGINSERVICE_H

#include <thread>
#include "MessageQueue.h"
#include "ServiceRequest.h"
#include "PluginConfig.h"
#include "PluginCallbackInterface.h"
class VirtualFSProvider;

namespace streamfs {

class PluginService {
public:
    PluginService(PluginCallbackInterface* cb) :mCb(cb){}
    void start();

    /**
     * Update plugin configuration
     * (called by plugin)
     */
    void updateConfig(PluginConfig &config) {
        mCb->updateConfig(config);
    }

    void setAvailableStreams(const std::vector<std::string>& streamIds)
    {
        mCb->setAvailableStreams(streamIds);
    }

    const std::vector<std::string> & getAvailableStreams() {
        auto& res = mCb->getAvailableStreams();
        return res;
    }

    /**
    * Get plugin identifier
    * @return
    */
    virtual std::string getId() = 0;

    /**
     * Stop play
     */
    virtual void stopPlayback() = 0;

    virtual int read(uint64_t handle, std::string path, char *buf, size_t size, uint64_t offset) =  0;

    virtual int open(std::string path)  =  0;

private:
    void loop();
    bool pendingExit();
private:
    std::thread *mThread{};
    MessageQueue<ServiceRequest> mServReq;
    PluginCallbackInterface* mCb;
};

}//namespace streamfs
#endif //STREAMLINK_PLUGINSERVICE_H
