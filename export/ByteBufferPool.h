//
// Created by Zoltan Kuscsik on 4/21/20.
//

#ifndef STREAMLINK_BYTEBUFFERPOOL_H
#define STREAMLINK_BYTEBUFFERPOOL_H

#include "BufferPool.h"

class ByteBufferPool : public BufferPool<buffer_chunk >{
public:
    typedef std::shared_ptr<BufferProducer<buffer_chunk>>  shared_producer_type;
    typedef std::shared_ptr<BufferConsumer<buffer_chunk>> shared_consumer_type;

    ByteBufferPool(
            shared_producer_type pProducer,
            shared_consumer_type pConsumer,
            uint64_t i);

    void read(
            BufferList* bufferChunks,
            size_t length,
            uint64_t offset) override;

    void pushBuffer(buffer_chunk &buffer) override;

};


#endif //STREAMLINK_BYTEBUFFERPOOL_H
