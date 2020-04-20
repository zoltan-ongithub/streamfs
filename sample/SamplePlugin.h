//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_SAMPLEPLUGIN_H
#define STREAMFS_SAMPLEPLUGIN_H

#include <PluginInterface.h>
#include <memory>
#include <string>

namespace streamfs {

class SamplePlugin : public PluginInterface {
public:
    std::string getId() override;

    void startPlayback() override;

    void stopPlayback() override;

    void updateConfiguration(const PluginConfig &config) override;

    void registerCallback(std::weak_ptr<PluginCallbackInterface> cb) override;

private:
    std::weak_ptr<PluginCallbackInterface> mCb;
    PluginConfig mConfig;
};

}

extern "C" {
streamfs::PluginInterface *INIT_STREAMFS_PLUGIN() {
    return new streamfs::SamplePlugin();
}
}

#endif //STREAMFS_SAMPLEPLUGIN_H
