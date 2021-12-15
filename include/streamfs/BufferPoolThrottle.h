#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/thread/condition.hpp>
#include <thread>
#include <atomic>

#include "streamfs/StopWatchTimer.h"
#include "streamfs/ConditionVariableHelper.h"

// Number of considered throttle loop iterations where the read position
// is allowed to be unchanged before the throttle loop will stop.
#define IDLE_LIMIT_COUNT 100
// Number of initial buffers allowed to be read by the player without
// applying throttling after a channel or seek has change.
#define READ_AHEAD_COUNT 30
// The buffer chunk time period truncation threshold in us. A registered
// time period less than this value will be truncated to 0 us. Likewise,
// registered time periods greater than this value will be reduced
// accordingly as a mean to compensate for additional time comming
// from code execution.
#define TIME_PERIOD_TRUNCATION_THRESHOLD_US 500
// The delta index between the time period buffer size and current read
// position defining when to apply throttling / assuming TSB playback,
// If the delta index is greater than this value, then throttle will
// be applied (assuming TSB playback) otherwise not (assuming
// live-playback)
#define LIVE_POSITION_THRESHOLD_INDEX 10

/*
 * This class implements throttle logic to facilitate a mechanism used for
 * ensuring that the rate at which buffers can be read from the TSB are
 * aligned with the rate at which they injected into the TSB for a certain
 * buffer chunk position.
 *
 * The class implements a throttle loop, which maintains a throttle position index
 * (mThrottleIndex), indicating the current buffer chunk being throttled. For a
 * certain throttle position index, it fetches the associated period from the
 * time period buffer (mTimePeriodBuffer). This time period indicates the delta
 * time relative to the arrival of the previous buffer chunk and is used for
 * initiating a blocking sleep.
 *
 * When the sleep completes, the throttle index is incremented and the procedure
 * is repeated in a loop. This procedure ensures that the rate at which
 * mThrottleIndex is incremented thereby follows the rate at which buffers are
 * pushed to the buffer pool, which is used in conjunction facilitating the
 * throttling.
 *
 * The class provides the following method related to the throttling:
 *
 * - registerTimePeriod: sample-and-register buffer chunk period time which will
 *   be pushed to the time period buffer. Shall be called whenever a new buffer
 *   chunk is received.
 *
 * - setReadPosition: sets the read positions targeting for throttling.
 *
 * - wait: executes the actual throttling for the read position set. The method
 *   will block until the throttle index is larger than the read position.
 *
 * See method description for further elaboration.
 */
class BufferPoolThrottle
{
public:
    BufferPoolThrottle(uint64_t preAllocBufSize);
    ~BufferPoolThrottle();

    /**
     * Measures and pushes buffer chunk time period into the time period buffer.
     * The time period will be measured relative to the last time
     * registerTimePeriod() was called.
     *
     * MUST be called when a new buffer chunk is received to ensure correct
     * time period registration.
     *
     * The initial time period will be zero for the first buffer ever or after
     * clear() or clearToLastRead() has been called.
     */
    void registerTimePeriod();

    /**
     * Set read position for throttling. Throttling is executed when
     * calling wait-method afterwards.
     *
     * @param pos - the read position index into the circular buffer.
     */
    void setReadPosition(uint64_t pos);

    /**
     * Initiate throttling for read position set using setReadPosition. The
     * method will block until throttling is completed for the read position.
     * Throttling completes when/if the throttle index (mThrottleIndex) is
     * larger than the read position.
     *
     * If a discontinuous change in the read position is detected (e.g. due
     * to a search) the throttle position index (mThrottleIndex) will be
     * re-initialized to the new read position.
     */
    void wait();

    /**
     * Clear time period buffer. Will also clear delay time measure used in
     * registerDelayTime() and related variables.
     */
    void clear();

    /**
     * Clear time period buffer until the last read. Will also clear delay
     * time measure used in registerDelayTime() and related variables.
     * Can be called only from the consumer thread.
     */
    void clearToLastRead();

private:
    boost::circular_buffer<uint32_t> mTimePeriodBuffer;
    StopWatchTimer mStopWatchTimer;
    std::shared_ptr<std::thread> mThrottleThread;

    uint16_t mIdleCount;
    uint64_t mLastPosIdle;

    std::atomic<uint64_t> mPos;
    std::atomic<uint64_t> mLastPos;

    std::atomic<bool> mExitRequested;
    std::atomic<bool> mThrottleRunning;
    std::atomic<uint64_t> mThrottleIndex;
    std::atomic<uint64_t> mReadAheadCount;

    ConditionVariableHelper mCvStartThrottle;
    ConditionVariableHelper mCvThrottleChanged;
    ConditionVariableHelper mCvTimePeriodRegistered;

    boost::mutex mMutexTimePeriodBuffer;
    boost::mutex mMutexSetReadPosition;

    void throttleLoop();
};
