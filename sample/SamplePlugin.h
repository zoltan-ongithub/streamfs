//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_SAMPLEPLUGIN_H
#define STREAMFS_SAMPLEPLUGIN_H

#include <PluginInterface.h>
#include <memory>
#include <string>
#include <BufferProducer.h>

namespace streamfs {

class SamplePlugin : public PluginInterface,  public BufferProducer<buffer_chunk> {
public:
    std::string getId() override;

    void startPlayback(std::string uri) override;

    void stopPlayback() override;

    void updateConfiguration(const PluginConfig &config) override;

    void registerCallback(std::weak_ptr<PluginCallbackInterface> cb) override;

    BufferProducer<buffer_chunk> *getBufferProducer() override;

private:
    std::weak_ptr<PluginCallbackInterface> mCb;
    PluginConfig mConfig{};
};

}

extern "C" {
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN() {
    return new streamfs::SamplePlugin();
}
}

#endif //STREAMFS_SAMPLEPLUGIN_H
