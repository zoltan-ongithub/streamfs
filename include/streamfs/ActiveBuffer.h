//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_ACTIVEBUFFER_H
#define STREAMFS_ACTIVEBUFFER_H


#include <stdint-gcc.h>

enum playback_status {
    STOPPED,
    PLAYING,
    CONFIGURING,
    SEEKING,
    ERROR,
    STOPPING,
    PAUSED,
};

class ActiveBuffer {
    playback_status currentStatus;
};

#endif //STREAMFS_ACTIVEBUFFER_H
