//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPRODUCER_H
#define STREAMFS_BUFFERPRODUCER_H

#include <cstddef>

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

        mBufferPool = bufferPool;
    }
private:
    BufferPool<T> *mBufferPool;
};

#endif //STREAMFS_BUFFERPRODUCER_H
