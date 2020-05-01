//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include "ByteBufferPool.h"

#include <utility>

void ByteBufferPool::pushBuffer(buffer_chunk &buffer) {
    BufferPool::pushBuffer(buffer);
}

ByteBufferPool::ByteBufferPool(ByteBufferPool::shared_producer_type pProducer,
                               ByteBufferPool::shared_consumer_type pConsumer, uint64_t i) :
        BufferPool(std::move(pProducer), std::move(pConsumer), i){

}

void ByteBufferPool::read(BufferList *bufferChunks, size_t length, uint64_t offset) {
    BufferPool::read(bufferChunks, length, offset);
}

