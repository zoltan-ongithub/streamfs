//
// Created by Zoltan Kuscsik on 4/24/20.
//

#include "VirtualFSProvider.h"
//#include "PluginCbImpl.h"
#include "PluginCallbackInterface.h"
#include "PluginInterface.h"

std::vector<VirtualFSProvider::FileNode> VirtualFSProvider::getNodes() {
    std::lock_guard<std::mutex> mLock(mFileListLock);
    std::vector<FileNode> nodes;

    auto inf = mCb.lock();
    auto offset = 0;

    if (inf) {
        auto fileList = inf->getAvailableStreams();
        nodes.resize(fileList.size() + mSubProviders.size());
        for( const auto &provider: fileList ){
            nodes[offset].type = NodeTypes::FILE_TYPE;
            nodes[offset].provider = this;
            nodes[offset].name = provider;
            offset++;
        }
    } else {
        nodes.resize(mSubProviders.size());
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

VirtualFSProvider::VirtualFSProvider(const std::string &name, std::weak_ptr<streamfs::PluginInterface> cb,
                                     FileInterface &fileInterface, bool isPlugin)
        : mName(name), mCb(cb),
                                       mFileInteface(fileInterface),
                                       mPluginInterface(nullptr),
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
int VirtualFSProvider::read(std::string node,
                            char *buf,
                            size_t size,
                            uint64_t offset) {
    auto x = mCb.lock();

    if (x) {
        return x->read(node, buf, size, offset);
    }
    return -ENOENT;

}
int VirtualFSProvider::open(std::string basicString) {

    auto x = mCb.lock();

    if (x) {
        return x->open(basicString);
    }
    return -ENOENT;
}

int VirtualFSProvider::write(std::string node, const char *buf, size_t size, uint64_t offset) {
    auto x = mCb.lock();

    if (x) {
        return x->write(node, buf, size, offset);
    }

    return -1;
}
