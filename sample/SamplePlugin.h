//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_SAMPLEPLUGIN_H
#define STREAMFS_SAMPLEPLUGIN_H

#include <streamfs/PluginInterface.h>
#include <memory>
#include <string>
#include <utility>
#include <streamfs/BufferConsumer.h>
#include <streamfs/ByteBufferPool.h>
#include "streamfs/PluginCallbackInterface.h"

namespace streamfs {

class SamplePlugin : public PluginInterface {
    typedef std::string uri_type;
    std::map<uri_type, std::shared_ptr<ByteBufferPool>> mBufferPool;

public:
    explicit SamplePlugin(PluginCallbackInterface* cb);

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
};

}

extern "C" {
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN(streamfs::PluginCallbackInterface* cb) {
    return new streamfs::SamplePlugin(cb);
}
}

#endif //STREAMFS_SAMPLEPLUGIN_H
