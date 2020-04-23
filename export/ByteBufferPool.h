//
// Created by Zoltan Kuscsik on 4/21/20.
//

#ifndef STREAMLINK_BYTEBUFFERPOOL_H
#define STREAMLINK_BYTEBUFFERPOOL_H

#include "BufferPool.h"

class ByteBufferPool : public BufferPool<buffer_chunk >{
public:
    ByteBufferPool(BufferProducer<buffer_chunk> *pProducer, BufferConsumer<buffer_chunk> *pConsumer, uint64_t i);

    void readHead(BufferList &bufferChunks, const BufferList *lastChunks) override;

    void pushBuffer(buffer_chunk &buffer) override;

};


#endif //STREAMLINK_BYTEBUFFERPOOL_H
