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
#include "PluginCallbackInterface.h"

namespace streamfs {
class PluginService;

/**
 * Generic plugin interface for 3rd party plugins
 */
class PluginInterface : public PluginService {
public:
    PluginInterface(PluginCallbackInterface* cb) : PluginService(cb), mCb(cb) {};

    /**
   * Update plugin configuration. Called first time before `registerCallback`
   * @param config - configuration
   */
    virtual void updateConfiguration(const PluginConfig &config) = 0;

    /**
     * Get buffer producer.
     * @return pointer to buffer producer.
     */
    virtual BufferProducer<buffer_chunk> *getBufferProducer() = 0;

    /**
     * Get plugin identifier
     * @return
     */
    virtual std::string getId() = 0;

    /**
     * Stop play
     */
    virtual void stopPlayback() = 0;

    virtual int open(std::string path)  =  0;

    virtual int read(std::string path,  char *buf,
                     size_t size, uint64_t offset)  =  0;

    PluginCallbackInterface* mCb;

};

}

extern "C" {
// Implement function in plugin to create a new plugin interface
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN(streamfs::PluginCallbackInterface* cb);
}
#endif //STREAMFS_PLUGININTERFACE_H
