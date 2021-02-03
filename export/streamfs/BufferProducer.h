//
// Created by Zoltan Kuscsik on 4/20/20.
//

#pragma once

#include <cstddef>
#include <boost/thread/mutex.hpp>

template <typename T> class BufferPool;

template <class T>
class BufferProducer {
    template <typename T1>
    friend  class BufferPool;

public:
    virtual void queueBuffer(const T &buffer, bool lastBuffer = false, size_t lastBufferSize = 0) {
        mBufferPool->pushBuffer(buffer, lastBuffer, lastBufferSize);
    }

    virtual void stop();

    virtual size_t getTotalBufferCount() {
        boost::mutex::scoped_lock lock(m_mutex);
        return mBufferPool->getTotalBufferCount();
    }

protected:
    void setBufferPool(BufferPool<T> *bufferPool) {
        boost::mutex::scoped_lock lock(m_mutex);
        mBufferPool = bufferPool;
    }

private:
    BufferPool<T> *mBufferPool;
    boost::mutex m_mutex;
};