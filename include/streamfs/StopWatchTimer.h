#pragma once

#include <chrono>

class StopWatchTimer {
public:

    StopWatchTimer() : mResetTimer(true) {};

    void resetTimer() {
        mResetTimer = true;
    }

    uint32_t getLapTime() {
        uint32_t lapTime = 0;

        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

        if (!mResetTimer) {
            std::chrono::microseconds diff = (std::chrono::duration_cast<std::chrono::microseconds>(currentTime-mLastTime));
            lapTime = diff.count();
        }
        mResetTimer = false;
        mLastTime = currentTime;
        return lapTime;
    }

private:
    bool mResetTimer;
    std::chrono::high_resolution_clock::time_point mLastTime;
};
