//
// Created by Zoltan Kuscsik on 9/29/21.
//

#pragma once

#include <glog/logging.h>
#include "streamfs/LogLevels.h"
#include "fuse/IFuse.h"

#define SLOG(x, level) LOG_IF(x, IFuse::mDebugOptions.debugLevel & (level))