//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_STREAMMANAGER_H
#define STREAMFS_STREAMMANAGER_H

#include <string>
#include "Stream.h"

namespace streamfs {
/**
 * Manages multiple parallel streams
 */
class StreamManager {
public:
    Stream &openStream(std::string plugin_id, std::string stream_id);
};
}

#endif //STREAMFS_STREAMMANAGER_H
