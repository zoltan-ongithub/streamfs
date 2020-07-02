#ifndef STREAMFS_HTTPSTREAMCONTROL_H
#define STREAMFS_HTTPSTREAMCONTROL_H

#include <PluginInterface.h>
#include <memory>
#include <string>
#include <utility>
#include <BufferConsumer.h>
#include <ByteBufferPool.h>
#include "PluginCallbackInterface.h"
#include <set>
#include <map>

namespace streamfs {

class HttpStreamControlPlugin : public PluginInterface {
    typedef std::string uri_type;
    std::map<uri_type, std::shared_ptr<ByteBufferPool>> mBufferPool;

public:
    explicit HttpStreamControlPlugin(PluginCallbackInterface* cb);

    std::string getId() override;

    int open(std::string uri) override;

    void stopPlayback() override;

    void updateConfiguration(const PluginConfig &config) override;

    void newBufferNotify(buffer_chunk &buffer);

    int read(std::string path, char *buf, size_t size, uint64_t offset) override;

    int write(std::string node, const char *buf, size_t size, uint64_t offset) override;

private:
    std::weak_ptr<streamfs::PluginCallbackInterface> mCb;
    PluginConfig mConfig{};
    std::vector<std::string> mAvailableStreams;
    ByteBufferPool::shared_consumer_type mConsumer;
    std::mutex mFopMutex;
    std::shared_ptr<class HttpStreamProducer> mProducer;
};

}

extern "C" {
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN(streamfs::PluginCallbackInterface* cb) {
    return new streamfs::HttpStreamControlPlugin(cb);
}
}

#endif //STREAMFS_HTTPSTREAMCONTROLPLUGIN_H
