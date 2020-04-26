//
// Created by Zoltan Kuscsik on 4/24/20.
//

#include "VirtualFSProvider.h"

std::vector<VirtualFSProvider::FileNode> VirtualFSProvider::getNodes() {
    std::lock_guard<std::mutex> mLock(mFileListLock);

    std::vector<FileNode> nodes(mFileList.size() + mSubProviders.size());
    auto offset = 0;

    for( const auto &provider: mFileList ){
        nodes[offset].type = NodeTypes::FILE_TYPE;
        nodes[offset].provider = this;
        nodes[offset].name = provider;
        offset++;
    }

    for (const auto & mSubProvider : mSubProviders) {
        nodes[offset].type = NodeTypes ::DIRECTORY_TYPE;
        nodes[offset].provider = mSubProvider.get();
        offset++;
    }

    return nodes;
}

VirtualFSProvider::~VirtualFSProvider() {
    /* TODO: Support removal. Fuse interface must be locked before removing a provider*/
    if (mIsPluginHandler) {
        mFileInteface.removeFsProvider(this->getName());
    }
}

VirtualFSProvider::VirtualFSProvider(const std::string &name, std::weak_ptr<VirtualFsCallbackHandler> cb,
                                     FileInterface& fileInterface,
                                     bool isPlugin)
                                     : mName(name), mCb(cb),
                                       mFileInteface(fileInterface),
                                       mIsPluginHandler(isPlugin) {
    if (name.empty()) {
        LOG(ERROR) << "Directory name can not be empty";
        throw;
    }
    if (cb.expired()) {
        LOG(ERROR) << "Callback handler can't be null";
        throw;
    }

    if (mIsPluginHandler) {
        fileInterface.registerFsProvider(this);
    }
}

