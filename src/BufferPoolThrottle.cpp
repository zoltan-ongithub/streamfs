#include "streamfs/BufferPoolThrottle.h"

#include <glog/logging.h>
#include <streamfs/LogLevels.h>
#include <streamfs/Logging.h>

#define BUFFER_POOL_READ_TIMEOUT_MS 1000

BufferPoolThrottle::BufferPoolThrottle(uint64_t preAllocBufSize)
    : mTimePeriodBuffer(preAllocBufSize)
    , mIdleCount(0)
    , mLastPosIdle(0)
    , mPos(0)
    , mLastPos(0)
    , mExitRequested(false)
    , mThrottleRunning(false)
    , mThrottleIndex(0)
    , mReadAheadCount(0) {
    SLOG(INFO, LOG_BUFFER_THROTTLE) << "throttle constructor";
    mThrottleThread = std::shared_ptr<std::thread>(
            new std::thread(&BufferPoolThrottle::throttleLoop, this));

    sched_param sch;

    // Set maximum thread priority for the throttle loop
    // TODO: does not seems to be applied even though we get the POSIX thread handle. Need to look into cgroups.
    int policy;
    pthread_getschedparam(mThrottleThread->native_handle(), &policy, &sch);
    sch.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (pthread_setschedparam(mThrottleThread->native_handle(), SCHED_FIFO, &sch)) {
        LOG(ERROR) << __FUNCTION__ << ": failed to setschedparam: " << std::strerror(errno) << std::flush;
    }

}

BufferPoolThrottle::~BufferPoolThrottle() {
    SLOG(INFO, LOG_BUFFER_THROTTLE) << "throttle destructor";
    mExitRequested = true;
    mThrottleRunning = false;

    mCvTimePeriodRegistered.notifyOne();
    mCvStartThrottle.notifyOne();
    mCvThrottleChanged.notifyOne();

    if (mThrottleThread != nullptr && mThrottleThread->joinable()) {
        mThrottleThread->join();
    }

    SLOG(INFO, LOG_BUFFER_THROTTLE) << "exit destructor";
}

void BufferPoolThrottle::registerTimePeriod() {
    boost::mutex::scoped_lock lockWrite(mMutexTimePeriodBuffer);
    auto time = mStopWatchTimer.getLapTime();

    // Truncate the delta time to zero if it is less than threshold value
    if (time < TIME_PERIOD_TRUNCATION_THRESHOLD_US) {
        time = 0;
    } else {
        time -= TIME_PERIOD_TRUNCATION_THRESHOLD_US;
    }

    // Decrement the throttle index and the current and last position to
    // compensate for buffer rotation, which will happen if the buffer
    // size and capacity are the same.
    if (mTimePeriodBuffer.size() == mTimePeriodBuffer.capacity()) {
        if (mThrottleIndex > 0) { --mThrottleIndex; }
        if (mPos > 0) { --mPos; }
    }

    mTimePeriodBuffer.push_back(time);
    mCvTimePeriodRegistered.notifyOne();
}

void BufferPoolThrottle::setReadPosition(uint64_t pos) {
    boost::mutex::scoped_lock lockWrite(mMutexSetReadPosition);
    mPos = pos;
}

void BufferPoolThrottle::wait() {
    boost::mutex::scoped_lock lockWrite(mMutexSetReadPosition);

    // If the delta position between the size of the time period
    // buffer and the read position index less or equal to the
    // LIVE_POSITION_THRESHOLD_INDEX, it will be considered as
    // Live playback meaning that throttle will not be applied.
    if (mTimePeriodBuffer.size() - mPos <= LIVE_POSITION_THRESHOLD_INDEX) {
        mLastPos.store(mPos.load());
        // Make sure the throttle loop will stop if it is running.
        mThrottleRunning = false;
        return;
    }

    if (mReadAheadCount < READ_AHEAD_COUNT) {
        ++mReadAheadCount;
    }

    SLOG(INFO, LOG_BUFFER_THROTTLE) << "fetch buffer mPos=" << mPos << " mThrottleIndex=" << mThrottleIndex;

    if (mReadAheadCount > READ_AHEAD_COUNT) {
        if (!mThrottleRunning) {
            mThrottleIndex.store(mPos.load());
            mThrottleRunning = true;
            SLOG(INFO, LOG_BUFFER_THROTTLE) << "notify mCvStartThrottle";
            mCvStartThrottle.notifyOne();
        } else if (abs(mPos - mLastPos) > 10) {
            mReadAheadCount = 0;
            mThrottleIndex.store(mPos.load());
            SLOG(INFO, LOG_BUFFER_THROTTLE)<< "Discontinuous read position detected! Likely it's a search. Changed mThrottleIndex=" << mThrottleIndex;
        }

        // Wait for throttling to complete if the index position is greater than or equal to the throttle index
        // or the throttle loop is no longer running (e.g. triggered by the throttle loop itself if a timeout
        // occurs while waiting for buffers - see wait condition in the throttle loop)
        while (mPos >= mThrottleIndex && mThrottleRunning) {
            SLOG(INFO, LOG_BUFFER_THROTTLE) << "waiting mPos=" << mPos << " mThrottleIndex=" <<mThrottleIndex;
            mCvThrottleChanged.wait([this](){ return mPos < mThrottleIndex || !mThrottleRunning; });
            SLOG(INFO, LOG_BUFFER_THROTTLE) << "mThrottleIndex=" << mThrottleIndex;
        }
    } else if (mReadAheadCount == READ_AHEAD_COUNT) {
        SLOG(INFO, LOG_BUFFER_THROTTLE) << "readahead completed: " << mReadAheadCount;
        ++mReadAheadCount;
        mThrottleIndex.store(mPos.load());
    }
    mLastPos.store(mPos.load());
}

void BufferPoolThrottle::clear() {
    boost::mutex::scoped_lock lockWrite(mMutexTimePeriodBuffer);
    mTimePeriodBuffer.clear();
    mStopWatchTimer.resetTimer();
    mReadAheadCount = 0;
    mThrottleIndex = 0;
    mPos = 0;
    mLastPos = 0;
    mThrottleRunning = false;
}

void BufferPoolThrottle::clearToLastRead() {
    boost::mutex::scoped_lock lockWrite(mMutexTimePeriodBuffer);

    if (mTimePeriodBuffer.empty())
        return;

    auto numElements = mTimePeriodBuffer.size();
    if (mLastPos < numElements) {
        auto eraseCount = numElements - mLastPos - 1;
        mTimePeriodBuffer.erase_end(eraseCount);
    }

    mStopWatchTimer.resetTimer();
    mReadAheadCount = 0;
    mThrottleIndex = 0;
    mThrottleRunning = false;
}

void BufferPoolThrottle::throttleLoop() {
    SLOG(INFO, LOG_BUFFER_THROTTLE) << "   throttleLoop thread started";
    while (!mExitRequested) {

        mIdleCount = 0;

        if (!mThrottleRunning) {
            SLOG(INFO, LOG_BUFFER_THROTTLE) << "   waiting for mCvStartThrottle";
            mCvStartThrottle.wait([this](){ return mExitRequested.load(); });
        }

        SLOG(INFO, LOG_BUFFER_THROTTLE) << "   starting throttle loop";
        while (mThrottleRunning && !mExitRequested) {
            bool notifyThrottleIndexChanged = false;
            uint32_t time = 0;
            {
                // If the current throttle index is within the time period buffer
                // range, then fetch the buffer arrival latency from the time period
                // buffer. This will be used for the thread sleep below.
                boost::mutex::scoped_lock lock(mMutexTimePeriodBuffer);
                if(mThrottleIndex < mTimePeriodBuffer.size()) {
                    time = mTimePeriodBuffer[mThrottleIndex];
                    notifyThrottleIndexChanged = true;
                    SLOG(INFO, LOG_BUFFER_THROTTLE) << "   fetch time=" << time << " for mThrottleIndex=" << mThrottleIndex;
                }
            }

            if (notifyThrottleIndexChanged == false) {
                // If notifyThrottleIndexChanged is false, it means that
                // the current throttle index is out of range (in particular it will be
                // the size of the time period buffer). Then we wait for a new buffer to
                // arrive or for the wait to timeout (e.g. due to buffer chunks injection
                // stops), in which case we stop the throttle loop by setting
                // mThrottleRunning to false.
                SLOG(INFO, LOG_BUFFER_THROTTLE) << "   not enough data. mThrottleIndex=" << mThrottleIndex << " size=" << mTimePeriodBuffer.size();
                if (!mCvTimePeriodRegistered.wait_for(BUFFER_POOL_READ_TIMEOUT_MS, [this](){ return mThrottleIndex < mTimePeriodBuffer.size(); })) {
                    SLOG(INFO, LOG_BUFFER_THROTTLE) << "   ### timed out while waiting for new buffer! ###";
                    mThrottleRunning = false;
                    mCvThrottleChanged.notifyOne();
                } else {
                    SLOG(INFO, LOG_BUFFER_THROTTLE) << "   New buffer received! mThrottleIndex=" << mThrottleIndex << " size=" << mTimePeriodBuffer.size();
                }
            } else {
                // If we are here, it means that we have fetched the buffer chunk
                // time period associated with the current throttle index.
                // We now sleep the amount of time corresponding to this.
                std::this_thread::sleep_for(std::chrono::microseconds(time));
                SLOG(INFO, LOG_BUFFER_THROTTLE) << "   sleep completed! mThrottleIndex=" << mThrottleIndex;
                // ... and then increment the throttle index to point to the
                // next time period in the time period buffer.
                ++mThrottleIndex;
                // ... and notify / unblock the wait in the wait-method if it is blocking.
                mCvThrottleChanged.notifyOne();

                if (mLastPos == mLastPosIdle) {
                    ++mIdleCount;
                    SLOG(INFO, LOG_BUFFER_THROTTLE) << "   read position not changed! mLastPos=" << mLastPos << " mLastPosIdle=" << mLastPosIdle << " mIdleCount=" << mIdleCount;
                } else {
                    mIdleCount = 0;
                }
            }

            SLOG(INFO, LOG_BUFFER_THROTTLE) << "   mLastPos=" << mLastPos << " mLastPosIdle=" << mLastPosIdle << " mIdleCount=" << mIdleCount;
            mLastPosIdle = mLastPos;
            {
                if (mIdleCount > IDLE_LIMIT_COUNT) {
                    SLOG(INFO, LOG_BUFFER_THROTTLE) << "Stopping throttling. mIdleCount=" << mIdleCount << " " << mThrottleIndex << ">=" << mTimePeriodBuffer.size();
                    mThrottleRunning = false;
                    mCvThrottleChanged.notifyOne();
                }
            }
        }
    }
    SLOG(INFO, LOG_BUFFER_THROTTLE) << "throttleLoop thread completed";
}
