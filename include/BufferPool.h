//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPOOL_H
#define STREAMFS_BUFFERPOOL_H

#include <boost/circular_buffer.hpp>
#include <config.h>
#include "BufferProducer.h"
#include "BufferConsumer.h"

typedef int8_t  buffer_chunk[BUFFER_CHUNK_SIZE];

struct BufferList{
    buffer_chunk* chunks;
    uint64_t length;
};
template <typename T> class BufferProducer;
template <typename T> class BufferConsumer;

template <class T>
class BufferPool {

public:
    explicit BufferPool(BufferProducer<T> producer, BufferConsumer<T> consumer, uint64_t preallocBufSize) :
        mProducer(producer),
        mConsumer(consumer),
        mCircBuf(preallocBufSize) {
        mProducer.setBufferPool(this);
    }
    /**
     * Read buffer until the head or MAX_BUFFER_LIST_COUNT number of chunks
     * @param bufferChunk
     * @param lastChunk - if last chunk is null, we will read the last buffer chunk.
     */
    virtual void readHead(BufferList& bufferChunks, const BufferList* lastChunks);

    virtual void pushBuffer(T& buffer);

private:
    BufferProducer<T> &mProducer;
    BufferConsumer<T> &mConsumer;
    boost::circular_buffer<T > mCircBuf;
};


#endif //STREAMFS_BUFFERPOOL_H
