//
// Created by Zoltan Kuscsik on 9/29/21.
//

#pragma once

// Buffer pool logs
#define LOG_BUFFER_POOL            (1 << 0)
// Buffer throttle logs
#define LOG_BUFFER_THROTTLE        (1 << 1)
// Configuration read/writes
#define LOG_CONFIGS                (1 << 2)
// Configuration data source logs
#define LOG_DATA_SRC               (1 << 3)
// Configuration data parser logs (PSI parser)
#define LOG_DATA_PARSER            (1 << 4)
// Plugin management logs
#define LOG_PLUGIN_MGR             (1 << 5)
// Video protection logs
#define LOG_PROTECTION             (1 << 6)