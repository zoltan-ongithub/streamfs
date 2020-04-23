//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include "ByteBufferPool.h"

void ByteBufferPool::readHead(BufferList &bufferChunks, const BufferList *lastChunks) {

}

void ByteBufferPool::pushBuffer(buffer_chunk &buffer) {

}

ByteBufferPool::ByteBufferPool(BufferProducer<buffer_chunk> *pProducer, BufferConsumer<buffer_chunk> *pConsumer,
                               uint64_t i) : BufferPool(pProducer, pConsumer, i) {
}
