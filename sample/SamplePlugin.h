//
// Created by Zoltan Kuscsik on 4/17/20.
//

#pragma once

#include <streamfs/PluginInterface.h>
#include <memory>
#include <string>
#include <utility>
#include <streamfs/BufferConsumer.h>
#include <streamfs/ByteBufferPool.h>
#include "streamfs/PluginCallbackInterface.h"
#include "fuse/IFuse.h"

namespace streamfs {

class SamplePlugin : public PluginInterface {
    typedef std::string uri_type;
    std::map<uri_type, std::shared_ptr<ByteBufferPool>> mBufferPool;

public:
    explicit SamplePlugin(PluginCallbackInterface* cb,  debug_options_t *debugOptions);

    std::string getId() override;

    int open(std::string uri) override;

    void stopPlayback() override;

    void updateConfiguration(const PluginConfig &config) override;

    void newBufferNotify(buffer_chunk &buffer);

    int read(uint64_t handle, std::string path, char *buf, size_t size, uint64_t offset) override;

    int write(std::string node, const char *buf, size_t size, uint64_t offset) override;

    int release(uint64_t handle, std::string path) override;

    uint64_t getSize(std::string path) override;

private:
    std::weak_ptr<streamfs::PluginCallbackInterface> mCb;
    PluginConfig mConfig{};
    std::vector<std::string> mAvailableStreams;
    ByteBufferPool::shared_consumer_type mConsumer;
    std::mutex mFopMutex;
};

}

extern "C" {
    streamfs::PluginInterface *INIT_STREAMFS_PLUGIN(streamfs::PluginCallbackInterface *cb, debug_options_t *debugOptions) {
        return new streamfs::SamplePlugin(cb, debugOptions);
}

const char *GET_STREAMFS_PLUGIN_ID() {
    return "sample_plugin";
}

void UNLOAD_STREAMFS_PLUGIN(streamfs::PluginInterface *mSample) {
}

}
