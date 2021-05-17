//
// Created by Zoltan Kuscsik on 4/17/20.
//
#include <iostream>
#include <cstring>
#include <memory>
#include <curl/curl.h>
#include "streamfs/ByteBufferPool.h"
#include "SamplePlugin.h"
#include <algorithm>

#define BUFFER_POOL_SIZE  100 * 1024

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

std::mutex writeLock;

extern "C" size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

class SampleProducer : public BufferProducer<buffer_chunk > {
public:
    SampleProducer(std::string uri) : mUri(uri), mThreadLoop(nullptr), mMustExit(false), mCurrentOffset(0){
        curl_global_init(CURL_GLOBAL_ALL);
    }

    void readFile() {
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

    void loop() {
        auto* curl_handle = curl_easy_init();
       // curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, BUFFER_CHUNK_SIZE);
        curl_easy_setopt(curl_handle, CURLOPT_URL,  "http://media.rdk.tvlab.cloud/TS/DR2.ts");
        //curl_easy_setopt(curl_handle, CURLOPT_URL,  "http://localhost:8080");

        auto result = curl_easy_perform(curl_handle);

        queueBuffer(tempBuffer, true, mCurrentOffset);

        printf("Exit with error %d\n", result);
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
    }

    void start() {
        if (mThreadLoop != nullptr) {
            mThreadLoop->join();
        }

        mThreadLoop = std::make_unique<std::thread>(&SampleProducer::loop, this);
    };

    ~SampleProducer() {
    }

    size_t write(void *ptr, size_t size, size_t nmemb) {

        auto total_size = nmemb * size;
        auto remaining_data = total_size;

        while (remaining_data > 0) {
            auto freeBytesInTempBuf = BUFFER_CHUNK_SIZE - mCurrentOffset;

            auto writeLength = std::min(freeBytesInTempBuf, remaining_data);

            memcpy(&tempBuffer[mCurrentOffset], &((char*)ptr )[ total_size - remaining_data], writeLength);
            mCurrentOffset += writeLength;

            if (mCurrentOffset == BUFFER_CHUNK_SIZE) {
                queueBuffer(tempBuffer, false, tempBuffer.size());
                mCurrentOffset = 0;
            }

            remaining_data -= writeLength;
        }
        return size * nmemb;
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
    buffer_chunk tempBuffer;
    size_t mCurrentOffset;
    bool mMustExit;
};

extern "C" size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::lock_guard<std::mutex> lockGuard(writeLock);
    SampleProducer* producer = (SampleProducer*) stream;
    return producer->write(ptr, size, nmemb);
}

SamplePlugin::SamplePlugin(PluginCallbackInterface* cb) :
    PluginInterface(cb) {
        mAvailableStreams.emplace_back("test1.tst");
        mAvailableStreams.emplace_back("test2.tst");
        mAvailableStreams.emplace_back("test3.tst");

    setAvailableStreams(mAvailableStreams);
}

std::string SamplePlugin::getId() {
    return "sample_plugin";
}
static int  read_offset_c = 0;

int SamplePlugin::open(std::string uri) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);
    std::cout << "Opened path:" << uri << std::endl;
    read_offset_c = 0;
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

int SamplePlugin::read(uint64_t handle, std::string path, char *buf, size_t size, uint64_t offset) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);

    auto bQueue = mBufferPool.find(path);
    static int i = 0;

    if (read_offset_c%20 == 0 ) {
        printf("Reading offset %lu size%zud\n", offset, size);
    }

    read_offset_c++;

    if (bQueue == mBufferPool.end()) {
        std::cerr << "Could not find handler for path " << path;
        return 0;
    }

    auto res = bQueue->second->readRandomAccess(buf, size , offset);
    return res;
}

void SamplePlugin::newBufferNotify(buffer_chunk &buffer) {

}

int SamplePlugin::write(std::string node, const char *buf, size_t size, uint64_t offset) {
    return 0;
}

int SamplePlugin::release(uint64_t handle, std::string path) {
    return 0;
}

}
