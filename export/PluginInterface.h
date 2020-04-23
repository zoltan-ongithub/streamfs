//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_PLUGININTERFACE_H
#define STREAMFS_PLUGININTERFACE_H

#include <string>
#include <vector>
#include <memory>
#include "PluginService.h"
#include "BufferProducer.h"
#include "BufferPool.h"
#include "PluginConfig.h"
#include "PluginService.h"

namespace streamfs {

class PluginCallbackInterface {
public:
    virtual void setAvailableStreams(std::vector<std::string> streamIds) = 0;
    virtual void updateConfig(PluginConfig &config) = 0;
};

class PluginService;

/**
 * Generic plugin interface for 3rd party plugins
 */
class PluginInterface : public PluginService {
public:
    PluginInterface() : PluginService(this) {};
/**
 * Get plugin identifier
 * @return
 */
    virtual std::string getId() = 0;

/**
 * Register callbackBufferProducer.
 * Called before startPlayback once. Plugin is responsible to manage a local
 * reference to the week pointer.
 */
    virtual void registerCallback(std::weak_ptr<PluginCallbackInterface> cb) {
    }

/**
 * Start play
 */
    virtual void startPlayback(std::string uri) = 0;

/**
 * Stop play
 */
    virtual void stopPlayback() = 0;

  /**
 * Update plugin configuration. Called first time before `registerCallback`
 * @param config - configuration
 */
    virtual void updateConfiguration(const PluginConfig &config) = 0;

    /**
     * Get buffer producer.
     * @return pointer to buffer producer.
     */
    virtual BufferProducer<buffer_chunk>* getBufferProducer() = 0;

};
}

extern "C" {
// Implement function in plugin to create a new plugin interface
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN();
}
#endif //STREAMFS_PLUGININTERFACE_H
