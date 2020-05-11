//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERCONSUMER_H
#define STREAMFS_BUFFERCONSUMER_H

template  <class T>
class BufferConsumer {
public:
    /**
     * Consumer is notified for new buffer when new buffer is available.
     * This buffer is buffer is used when playing back head of the stream without
     * seek.
     * @param buffer - front buffer of circular buffer
     */
    virtual void newBufferAvailable(T& buffer) = 0;
};


#endif //STREAMFS_BUFFERCONSUMER_H
