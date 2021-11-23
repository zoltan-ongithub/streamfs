#pragma once
#include <atomic>

struct DebugOptions {
    std::atomic<uint32_t> debugLevel;
    std::atomic<bool> tsDumpEnable;
};

typedef DebugOptions debug_options_t;