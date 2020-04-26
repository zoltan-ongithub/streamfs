//
// Created by Zoltan Kuscsik on 4/24/20.
//

#ifndef STREAMLINK_VIRTUALFSPROVIDER_H
#define STREAMLINK_VIRTUALFSPROVIDER_H

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <glog/logging.h>
#include <mutex>
#include <FileInterface.h>

enum NodeTypes {
    FILE_TYPE,
    DIRECTORY_TYPE,
};

class VirtualFsCallbackHandler {

};

class VirtualFSProvider {
    typedef std::string fileId;
    typedef std::set<fileId> fileList;

    struct FileNode {
        std::string name;
        VirtualFSProvider *provider;
        NodeTypes type;
    };

public:
    VirtualFSProvider(const std::string& name,
            std::weak_ptr<VirtualFsCallbackHandler> cb,
            FileInterface& fileInterface,
            bool isPlugin);
    ~VirtualFSProvider();

    std::string getName() { return mName;}

    void updateAvailableFiles(fileList &fList) {
        std::lock_guard<std::mutex> lock(mFileListLock);
        mFileList = fList;
    }

    int attachProvider(VirtualFSProvider* provider) {
        if (provider == nullptr)
            return -1;

        if (mFileList.find(provider->getName()) != mFileList.end()) {
            LOG(ERROR) << "Can't attach new directory. File already exists:" << provider->getName();
            return -2;
        }

        mSubProviders.insert(std::unique_ptr<VirtualFSProvider>(provider));
        return 0;
    }

    std::vector<FileNode> getNodes();

    std::shared_ptr<VirtualFsCallbackHandler> getCallback() {
        return mCb.lock();
    }


private:
    fileList mFileList;
    std::weak_ptr<VirtualFsCallbackHandler> mCb;
    std::mutex mFileListLock;
    std::string mName;
    std::set<std::unique_ptr<VirtualFSProvider>> mSubProviders;
    FileInterface& mFileInteface;
    bool mIsPluginHandler;
};

#endif //STREAMLINK_VIRTUALFSPROVIDER_H
