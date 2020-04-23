//
// Created by Zoltan Kuscsik
//

#ifndef STREAMLINK_MESSAGEQUEUE_H
#define STREAMLINK_MESSAGEQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

/**
 * Generic message queue
 * @param T - message type
 */
template <class T>
class MessageQueue {

public:
    T waitForMessage() {
        std::unique_lock<std::mutex> lock(mMtx);
        T res;
        this->mCond.wait(lock, [=]{ return !mQueue.empty(); });
        res = mQueue.front();
        mQueue.pop();
        return res;
    }

    void pushMessage(T msg) {
        std::unique_lock<std::mutex> lock(mMtx);
        mQueue.push(msg);
        this->mCond.notify_one();
    }

    int getMessage(T &msg) {
        std::lock_guard<std::mutex> lock(mMtx);
        if(mQueue.empty()) {
            return 0;
        }
        msg = mQueue.front();
        mQueue.pop();
        return -1;
    }

private:
    std::condition_variable mCond;
    std::mutex mMtx;
    std::queue<T> mQueue;
};

#endif //STREAMLINK_MESSAGEQUEUE_H
