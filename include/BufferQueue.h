//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_BUFFERQUEUE_H
#define STREAMFS_BUFFERQUEUE_H

#include <cstdint>
#include <queue>
#include <condition_variable>

namespace streamfs {

#define QUEUE_SIZE    2

/**
 * Generic producer/consumer queue
 * @tparam T
 * @tparam queueSize
 */

template<typename T, unsigned long queueSize = QUEUE_SIZE>
class BufferQueue {
    std::condition_variable fillBuffCond;
    std::condition_variable emptyBuffCond;
    std::mutex fillM;
    std::mutex emptyM;
    std::queue<T *> consumerQueue;
    std::queue<T *> producerQueue;
    int mMaxDepth;
public:
    BufferQueue() : mMaxDepth(queueSize) {}

    uint32_t getQueueSize() { return mMaxDepth; }

    void queue(T *req) {
        std::unique_lock<std::mutex> lock(fillM);
        fillBuffCond.wait(lock, [this]() { return !isConsumerQueueFull(); });
        consumerQueue.push(req);
        lock.unlock();
        fillBuffCond.notify_all();
    }

    void acquire(T **req) {
        std::unique_lock<std::mutex> lock(emptyM);
        emptyBuffCond.wait(lock, [this]() { return !isProducerQueueEmpty(); });
        *req = producerQueue.front();
        producerQueue.pop();
        lock.unlock();
        emptyBuffCond.notify_all();
    }

    void consume(T **req) {
        std::unique_lock<std::mutex> lock(fillM);
        fillBuffCond.wait(lock, [this]() { return !isConsumerQueueEmpty(); });
        *req = consumerQueue.front();
        consumerQueue.pop();
        lock.unlock();
        fillBuffCond.notify_all();
    }

    bool consume(T **req, std::chrono::duration<int64_t> timeout) {
        std::unique_lock<std::mutex> lock(fillM);

        bool done = fillBuffCond.wait_for(lock, timeout, [this]() { return !isConsumerQueueEmpty(); });
        if (!done)
            return false;
        *req = consumerQueue.front();
        consumerQueue.pop();
        lock.unlock();
        fillBuffCond.notify_all();
        return true;
    }


    void release(T *req) {
        std::unique_lock<std::mutex> lock(emptyM);
        emptyBuffCond.wait(lock, [this]() { return !isProducerQueueFull(); });
        producerQueue.push(req);
        lock.unlock();
        emptyBuffCond.notify_all();
    }

    bool isConsumerQueueFull() const {
        return consumerQueue.size() >= mMaxDepth;
    }

    bool isConsumerQueueEmpty() const {
        return consumerQueue.size() == 0;
    }

    bool isProducerQueueFull() const {
        return producerQueue.size() >= mMaxDepth;
    }

    bool isProducerQueueEmpty() const {
        return producerQueue.size() == 0;
    }

    void clear() {
        std::unique_lock<std::mutex> lock(fillM);
        while (!isConsumerQueueEmpty()) {
            consumerQueue.pop();
        }
        fillBuffCond.notify_all();
        while (!isProducerQueueEmpty()) {
            producerQueue.pop();
        }
        lock.unlock();
        emptyBuffCond.notify_all();
    }
};
}

#endif //STREAMFS_BUFFERQUEUE_H
