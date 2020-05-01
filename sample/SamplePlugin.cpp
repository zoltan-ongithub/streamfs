//
// Created by Zoltan Kuscsik on 4/17/20.
//
#include <iostream>
#include <cstring>
#include "ByteBufferPool.h"
#include "SamplePlugin.h"
/* 20MB buffer pool. Very very good and very very cheap*/
#define BUFFER_POOL_SIZE 20 * 1024 * 1024

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
    std::cout << "Opened path:" << uri << std::endl;

    auto consumer = ByteBufferPool::shared_consumer_type(new SampleConsumer(this));
    auto producer = ByteBufferPool::shared_producer_type(new SampleProducer());
    auto mBufferPool = new ByteBufferPool(producer, consumer, BUFFER_POOL_SIZE);

    return 0;
}

void SamplePlugin::stopPlayback() {

}

void SamplePlugin::updateConfiguration(const PluginConfig &config) {
    mConfig = config;
}

int SamplePlugin::read(std::string path, char *buf, size_t size, uint64_t offset) {
    if (offset < 10000)
    {
        std::cout << "Reading! : " << size <<std::endl;
        memset(buf, 'x', size);
        return size;

    }

    return 0;
}

BufferProducer<buffer_chunk> *SamplePlugin::getBufferProducer() {
    return nullptr;
}

void SamplePlugin::newBufferNotify(buffer_chunk &buffer) {

}

}
