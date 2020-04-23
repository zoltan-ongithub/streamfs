//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPOOL_H
#define STREAMFS_BUFFERPOOL_H

#include <boost/circular_buffer.hpp>
#include <config.h>
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
template <class T>
class BufferPool {

public:
    /**
     * Buffer pool
     * @param producer - pointer to producer.
     * @param consumer  - pointer to consumer
     * @param preallocBufSize  - number of chunk memories to maintain.
     */
    explicit BufferPool(BufferProducer<T> *producer, BufferConsumer<T> *consumer, uint64_t preallocBufSize) :
        mProducer(producer),
        mConsumer(consumer),
        mCircBuf(preallocBufSize)
        {
        mProducer->setBufferPool(this);
    }
    /**
     * Read buffer until the head or MAX_BUFFER_LIST_COUNT number of chunks
     * @param bufferChunk
     * @param lastChunk - if last chunk is null, we will read the last buffer chunk.
     */
    virtual void readHead(BufferList& bufferChunks, const BufferList* lastChunks) = 0;

    virtual void pushBuffer(T& buffer);

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

private:
    std::shared_ptr<BufferProducer<T>> mProducer;
    std::shared_ptr<BufferConsumer<T>> mConsumer;
    boost::circular_buffer<T > mCircBuf;
};


#endif //STREAMFS_BUFFERPOOL_H
