//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include "ByteBufferPool.h"

#include <utility>

void ByteBufferPool::pushBuffer(buffer_chunk &buffer,  bool lastBuffer, size_t lastBufferSize) {
    BufferPool::pushBuffer(buffer, lastBuffer, lastBufferSize);
}

ByteBufferPool::ByteBufferPool(ByteBufferPool::shared_producer_type pProducer,
                               ByteBufferPool::shared_consumer_type pConsumer, uint64_t i) :
        BufferPool(std::move(pProducer), std::move(pConsumer), i){
}

size_t ByteBufferPool::read(char *bufferChunks, size_t length, uint64_t offset) {
    return BufferPool::read(bufferChunks, length, offset);
}

