//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPRODUCER_H
#define STREAMFS_BUFFERPRODUCER_H

#include <cstddef>
#include <boost/thread/mutex.hpp>

template <typename T> class BufferPool;

template <class T>
class BufferProducer {
    template <typename T1>
    friend  class BufferPool;

public:
    virtual void queueBuffer(T &buffer, bool lastBuffer = false, size_t lastBufferSize = 0) {
        mBufferPool->pushBuffer(buffer, lastBuffer, lastBufferSize);
    }

    virtual void stop();

protected:
    void setBufferPool(BufferPool<T> *bufferPool) {
        boost::mutex::scoped_lock lock(m_mutex);
        mBufferPool = bufferPool;
    }

    virtual size_t getTotalBufferCount() {
        size_t totalBufferCount;
        boost::mutex::scoped_lock lock(m_mutex);
        if (mBufferPool) {
            totalBufferCount =  mBufferPool->getTotalBufferCount();
        } else {
            totalBufferCount = 0;
        }
        return totalBufferCount;
    }
private:
    BufferPool<T> *mBufferPool;
    boost::mutex m_mutex;
};

#endif //STREAMFS_BUFFERPRODUCER_H
