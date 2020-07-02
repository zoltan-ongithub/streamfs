//
// Created by Zoltan Kuscsik on 4/23/20.
//

#ifndef STREAMLINK_SERVICEREQUEST_H
#define STREAMLINK_SERVICEREQUEST_H


#include <stdint-gcc.h>
#include <string>

enum ServiceRequestEnum {
    START,         // start playback. Caller must supply an uri
    STOP,          // on stop player should release all resources.
    PAUSE,         // Pause playback
    RELOAD,        // Reload stream configuration
    RECONFIGURE,   // Reconfigure plugin
    ABOUT_TO_KILL, // Plugin did not closed when stop called. This is the last warning.
    CUSTOM_REQUEST // Custom request reserved for plugins specific commands
};

class ServiceRequest {
    uint64_t  seekLocation;
    std::string customRequest;
public:
    ServiceRequestEnum type;
    std::string uri;
};

#endif //STREAMLINK_SERVICEREQUEST_H
