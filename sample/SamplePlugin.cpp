//
// Created by Zoltan Kuscsik on 4/17/20.
//
#include <iostream>
#include <cstring>
#include <memory>
#include <fcntl.h>
#include "ByteBufferPool.h"
#include "SamplePlugin.h"
/* 20MB buffer pool. Very very good and very very cheap*/
#define BUFFER_POOL_SIZE  1024 * 5

namespace streamfs {

class SampleConsumer : public BufferConsumer<buffer_chunk> {

public:
    void newBufferAvailable(buffer_chunk &buffer) override {
        mPlugin->newBufferNotify(buffer);
    }

    explicit SampleConsumer(SamplePlugin* plugin) : mPlugin(plugin) {}

private:
    SamplePlugin* mPlugin;
};

class SampleProducer : public BufferProducer<buffer_chunk > {
public:
    SampleProducer(std::string uri) : mUri(uri), mThreadLoop(nullptr), mMustExit(false){
    }

    void loop() {

        FILE* f = fopen("test.ts", "r");
        if (f == nullptr) {
            printf("Failed to open test.ts\n");
            return;
        }

        buffer_chunk chunk;
        size_t res;

        do {
            res = fread(chunk.data(), 1, chunk.size(), f);
            queueBuffer(chunk, res != chunk.size(), res);
            if (mMustExit) {
                fclose(f);
                return;
            }
        } while (res == chunk.size());
        fclose(f);
    }

    void start() {
        if (mThreadLoop != nullptr) {
            mThreadLoop->join();
        }

        mThreadLoop = std::make_unique<std::thread>(&SampleProducer::loop, this);
    };

    ~SampleProducer() {
    }

    void stop() override {
        BufferProducer::stop();
        if(mThreadLoop) {
            mThreadLoop->join();
        }
    }


private:
    std::string mUri;
    std::unique_ptr<std::thread> mThreadLoop;
    bool mMustExit;
};

SamplePlugin::SamplePlugin(PluginCallbackInterface* cb) :
    PluginInterface(cb) {
        mAvailableStreams.emplace_back("test1.tst");
        mAvailableStreams.emplace_back("test2.tst");
        setAvailableStreams(mAvailableStreams);
}

std::string SamplePlugin::getId() {
    return "sample_plugin";
}

int SamplePlugin::open(std::string uri) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);
    std::cout << "Opened path:" << uri << std::endl;
    if (mBufferPool.find(uri) != mBufferPool.end()) {
        return 0;
    }
    mBufferPool.erase(uri);

    auto p = new SampleProducer(uri);

    auto consumer = ByteBufferPool::shared_consumer_type(new SampleConsumer(this));
    auto producer = ByteBufferPool::shared_producer_type(p);

    mBufferPool.insert(std::make_pair(uri, std::make_shared<ByteBufferPool>( producer, consumer, BUFFER_POOL_SIZE)));

    p->start();
    return 0;
}

void SamplePlugin::stopPlayback() {

}

void SamplePlugin::updateConfiguration(const PluginConfig &config) {
    mConfig = config;
}

int SamplePlugin::read(std::string path, char *buf, size_t size, uint64_t offset) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);

    auto bQueue = mBufferPool.find(path);

    if (bQueue == mBufferPool.end()) {
        std::cerr << "Could not find handler for path " << path;
        return 0;
    }

    auto res = bQueue->second->readRandomAccess(buf, size , offset);
    return res;
}

BufferProducer<buffer_chunk> *SamplePlugin::getBufferProducer() {
    return nullptr;
}

void SamplePlugin::newBufferNotify(buffer_chunk &buffer) {

}

}
