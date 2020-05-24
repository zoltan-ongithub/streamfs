//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPOOL_H
#define STREAMFS_BUFFERPOOL_H

#include <boost/circular_buffer.hpp>
#include <boost/thread/condition.hpp>

#include "config.h"
#include <array>
#include "BufferProducer.h"
#include "BufferConsumer.h"

using buffer_chunk  = std::array<unsigned char, BUFFER_CHUNK_SIZE>;

struct BufferList{
    buffer_chunk* chunks;
    uint64_t length;
};

/**
 * Generic buffer pool.
 * Buffer pool is responsible to free the producer/consumer pointers.
 *
 * @tparam T
 */
template <typename T>
class BufferPool {

public:
    /**
     * Buffer pool
     * @param producer - pointer to producer.
     * @param consumer  - pointer to consumer
     * @param preallocBufSize  - number of chunk memories to maintain.
     */
    explicit BufferPool(
            std::shared_ptr<BufferProducer<T>> producer,
            std::shared_ptr<BufferConsumer<T>> consumer,
            uint64_t preallocBufSize):
            mReadEnd(0),
            mProducer(producer),
            mConsumer(consumer),
            mCircBuf(preallocBufSize),
            mTotalBufCount(0),
            mGotLastBuffer(false),
            mLastBufferSize(0)
        {
            mProducer->setBufferPool(this);
        }

    virtual size_t read(char* bufferChunks, size_t length, uint64_t offset, size_t left_padding , size_t right_padding ) = 0;

    virtual size_t readRandomAccess(char* data, size_t size, uint64_t offsetBytes)  = 0;

    virtual void pushBuffer(T& buffer,  bool lastBuffer = false, size_t lastBufferSize = 0);

    /**
     * Get circular buffer maximum capacity
     * @return
     */
    size_t getCapacity() {
        return mCircBuf.capacity();
    }

    /**
     * Get current size of the ring buffer
     * @return
     */
    size_t getSize() {
        return mCircBuf.size();
    }

    ~BufferPool() {
        mProducer->stop();
    }

private:
    bool hasEnoughBytes() const { return mReadEnd <= mTotalBufCount || mGotLastBuffer; }

    BufferPool(const BufferPool&);
    BufferPool& operator = (const BufferPool&);

private:
    std::shared_ptr<BufferProducer<T>> mProducer;
    std::shared_ptr<BufferConsumer<T>> mConsumer;
    boost::circular_buffer<T> mCircBuf;
    uint64_t mTotalBufCount;
    boost::mutex m_mutex;
    boost::mutex m_w_mutex;

    boost::condition mNotEnoughBytes;
    void lockWaitForRead();
    uint64_t mReadEnd;
    bool  mGotLastBuffer;
    size_t mLastBufferSize;
};


#endif //STREAMFS_BUFFERPOOL_H
