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

    size_t read(
            char* bufferChunks,
            size_t length,
            uint64_t offset,
            size_t left_padding,
            size_t right_padding
            ) override;

    size_t readRandomAccess(char* data, size_t size, uint64_t offsetBytes) override ;
    void pushBuffer(buffer_chunk &buffer,  bool lastBuffer = false, size_t lastBufferSize = 0) override;
    void clear() override;
};


#endif //STREAMLINK_BYTEBUFFERPOOL_H
