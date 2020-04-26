//
// Created by Zoltan Kuscsik on 4/24/20.
//

#include <cstring>
#include "fuse/IFuse.h"
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#define fs boost::filesystem

IFuse::fsProviderMapType IFuse::fsProviders;

IFuse::IFuse(){
}

fuse_operations *IFuse::getFuseOperations() {
    static fuse_operations fop = {
            .getattr = IFuse::getAttrCallback,
            .open = IFuse::openCallback,
            .read = IFuse::readCallback,
            .readdir = IFuse::readDirCallback
    };
    return &fop;
}

int
IFuse::getAttrCallback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    LOG(INFO) << "Inside getAttrCallback. path " << path;

    fs::path p(path);
    std::string pluginName;


    auto it = p.begin();
    std::advance(it, 1);

    if (it != p.begin()) {
        pluginName = it->string();
        std::advance(it, 1);
    }

    auto plugin = fsProviders.find(pluginName);

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }


    if ( !pluginName.empty() && plugin != fsProviders.end() && it == p.end()){
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        //stbuf->st_mode = S_IFREG | 0777;
        //stbuf->st_nlink = 1;
        //stbuf->st_size = 0;
        LOG(INFO) << "sample plugin ----->";
        return 0;
    }

    return -ENOENT;
}

int
IFuse::readDirCallback(
        const char *path,
        void *buf,
        fuse_fill_dir_t filler,
        off_t offset,
        struct fuse_file_info *fi) {

    fs::path p(path);
    LOG(INFO) << "Opening path:" << path;
    auto b = p.begin();

    auto root = b->c_str();

    auto pPair = fsProviders.find(root);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    if (strlen(path) == 1 && strncmp("/", path, 1) == 0) {
        for(auto& provider : fsProviders) {
                filler(buf, provider.first.c_str(), NULL, 0);
        }
    }

#if 0
    if (pPair == fsProviders.end()) {
        LOG(WARNING) << "Could not find provider for path" << path;
        return -ENOENT;
    }



    auto provider = pPair->second;

    LOG(INFO) << "Getting nodes for " << pPair->first;

    for (auto item : provider->getNodes()) {
        if (item.type == NodeTypes::FILE_TYPE) {
            filler(buf, item.name.c_str(), NULL, 0);
        }
    }
#endif
    return 0;
}

int IFuse::openCallback(const char *path,
        struct fuse_file_info *fi) {
    LOG(INFO) << "Inside openCallback";
    return 0;
}

int IFuse::readCallback(const char *path,
        char *buf,
        size_t size,
        off_t offset,
        struct fuse_file_info *fi) {
    LOG(INFO) << "Inside readCallback";

    return 0;
}

void IFuse::registerFsProvider(VirtualFSProvider* provider) {
    auto p = std::make_pair<>(provider->getName(),  provider);
    fsProviders.insert(p);
}

void IFuse::removeFsProvider(std::string id) {
    fsProviders.erase(id);
}
