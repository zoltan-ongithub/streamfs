//
// Created by Zoltan Kuscsik on 4/17/20.
//
#include <iostream>
#include <cstring>
#include <memory>
#include <curl/curl.h>
#include "ByteBufferPool.h"
#include "HttpStreamControlPlugin.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>


#define BUFFER_POOL_SIZE  100 * 1024

namespace streamfs {

class HttpStreamControlConsumer : public BufferConsumer<buffer_chunk> {

public:
    void newBufferAvailable(buffer_chunk &buffer) override {
        mPlugin->newBufferNotify(buffer);
    }

    explicit HttpStreamControlConsumer(HttpStreamControlPlugin* plugin) : mPlugin(plugin) {}

private:
    HttpStreamControlPlugin* mPlugin;
};

std::mutex writeLock;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

class HttpStreamProducer : public BufferProducer<buffer_chunk > {
public:
    HttpStreamProducer() : mThreadLoop(nullptr), mMustExit(false), mCurrentOffset(0){
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
        printf("In loop uri %s\n", mUri.c_str());

       // curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
        auto result = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, BUFFER_CHUNK_SIZE);
        curl_easy_setopt(curl_handle, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)(15*1024*1024/8));
        curl_easy_setopt(curl_handle, CURLOPT_URL,  mUri.c_str());
        //curl_easy_setopt(curl_handle, CURLOPT_URL,  "http://localhost:8080");

        while ((!mMustExit) && (CURLE_OK == result))
        {
            result = curl_easy_perform(curl_handle);
        }
        if (CURLE_OK == result)
        {
            printf("Fetch EOS\n");
            queueBuffer(tempBuffer, false, mCurrentOffset);
        }
        else
        {
            printf("Exit with error %d\n", result);
        }
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
    }

    void restart(std::string url) {
        if (mThreadLoop != nullptr) {
            mMustExit = true;
            mThreadLoop->join();
        }
        mUri = url;
        mMustExit = false;
        boost::trim(mUri);
        mThreadLoop = std::make_unique<std::thread>(&HttpStreamProducer::loop, this);
    };

    ~HttpStreamProducer() {
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
    std::atomic<bool>  mMustExit;

private:
    std::string mUri;
    std::unique_ptr<std::thread> mThreadLoop;
    buffer_chunk tempBuffer;
    size_t mCurrentOffset;
};

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::lock_guard<std::mutex> lockGuard(writeLock);
    HttpStreamProducer* producer = (HttpStreamProducer*) stream;
    if (producer->mMustExit)
    {
        std::cout << "Write data mMustExit set" << std::endl;
        return -1;
    }
    else
    {
        return producer->write(ptr, size, nmemb);
    }
}

HttpStreamControlPlugin::HttpStreamControlPlugin(PluginCallbackInterface* cb) :
    PluginInterface(cb) {
    mAvailableStreams.emplace_back("chan_select0");
    mAvailableStreams.emplace_back("stream0.ts");

    setAvailableStreams(mAvailableStreams);
}

std::string HttpStreamControlPlugin::getId() {
    return "httpstreamcontrol";
}
static int  read_offset_c = 0;

int HttpStreamControlPlugin::open(std::string uri) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);
    std::cout << "Opened path:" << uri << std::endl;
    read_offset_c = 0;
    if (mBufferPool.find(uri) != mBufferPool.end()) {
        return 0;
    }

    mBufferPool.erase(uri);

    if (nullptr == mProducer)
    {
        mProducer = std::make_shared< HttpStreamProducer>();
    }

    auto consumer = ByteBufferPool::shared_consumer_type(new HttpStreamControlConsumer(this));
    auto producer = ByteBufferPool::shared_producer_type(mProducer.get());

    mBufferPool.insert(std::make_pair(uri, std::make_shared<ByteBufferPool>( producer, consumer, BUFFER_POOL_SIZE)));
    return 0;
}

void HttpStreamControlPlugin::stopPlayback() {

}

void HttpStreamControlPlugin::updateConfiguration(const PluginConfig &config) {
    mConfig = config;
}

int HttpStreamControlPlugin::read(std::string path, char *buf, size_t size, uint64_t offset) {
    std::lock_guard<std::mutex> lockGuard(mFopMutex);

    auto bQueue = mBufferPool.find(path);
    static int i = 0;

    if (read_offset_c%20 == 0 ) {
//        printf("Reading offset %llu size%zud\n", offset, size);
    }

    read_offset_c++;

    if (bQueue == mBufferPool.end()) {
        std::cerr << "Could not find handler for path " << path;
        return 0;
    }

    auto res = bQueue->second->readRandomAccess(buf, size , offset);
    if (res != size)
    {
        printf("res %d size%zud offset %llu \n", res, size, offset);
    }
    return res;
}

void HttpStreamControlPlugin::newBufferNotify(buffer_chunk &buffer) {

}

int HttpStreamControlPlugin::write(std::string node, const char *buf, size_t size, uint64_t offset) {
    std::cerr << "HttpStreamControlPlugin::write " << buf;
    auto bQueue = mBufferPool.find(node);

    if (bQueue == mBufferPool.end()) {
        std::cerr << "Could not find handler for path " << node;
        return 0;
    }
    bQueue->second->clearToLastRead();
    mProducer->restart(buf);
    return size;
}

}
