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
        stbuf->st_nlink = 2;
        return 0;
    } else {
        auto provider = findProvider(path);

        if(provider != nullptr) {
            auto fsNodes = provider->getNodes();
            for(auto node : fsNodes) {
                if(node.name == it->string()){
                    switch (node.type) {
                        case FILE_TYPE:
                            stbuf->st_nlink = 2;
                            stbuf->st_size = UINT64_MAX;
                            stbuf->st_mode = S_IFREG | 0777;
                            return 0;
                        case DIRECTORY_TYPE:
                            stbuf->st_mode = S_IFDIR | 0755;
                            stbuf->st_nlink = 2;
                            return 0;
                    }
                }
            }
        }
    }


    return -ENOENT;
}

 VirtualFSProvider* IFuse::findProvider(const char* path) {
     fs::path p(path);
     auto b = p.begin();
     std::advance(b, 1);
     auto provider = fsProviders.find(b->string());

     if (provider !=  fsProviders.end()) {
         return provider->second;
     } else {
         return nullptr;
     }
}

int
IFuse::readDirCallback(
        const char *path,
        void *buf,
        fuse_fill_dir_t filler,
        off_t offset,
        struct fuse_file_info *fi) {

    fs::path p(path);
    auto b = p.begin();
    auto root = b->c_str();
    auto pPair = fsProviders.find(root);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    if (strlen(path) == 1 && strncmp("/", path, 1) == 0) {
        for(auto& provider : fsProviders) {
                filler(buf, provider.first.c_str(), NULL, 0);
        }
    }  else {
        auto provider = findProvider(path);
        if(provider != nullptr) {
            auto fsNodes = provider->getNodes();
            for(auto node : fsNodes) {
                filler(buf, node.name.c_str(), NULL, 0);
            }
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

int IFuse::openCallback(const char
        *path,
        struct fuse_file_info *fi) {

    auto provider = findProvider(path);
    fs::path p(path);
    auto b = p.begin();
    std::advance(b, 2);
    /**
     * TODO: add HASH based file name lookup.
     */
    if (provider != nullptr){
        auto fsNodes = provider->getNodes();

        for(auto node : fsNodes) {
            if(node.name == b->string()) {
                return provider->open(node.name);
            }
          }
    }

    return -ENOENT;
}

int IFuse::readCallback(const char *path,
        char *buf,
        size_t size,
        off_t offset,
        struct fuse_file_info *fi) {
    LOG(INFO) << "Inside readCallback";
    auto provider = findProvider(path);

    fs::path p(path);
    auto b = p.begin();
    std::advance(b, 2);

    /**
     * TODO: add HASH based file name lookup.
     */
    if (provider != nullptr){
        auto fsNodes = provider->getNodes();
        LOG(INFO) << "Found provider";
        for(auto node : fsNodes) {
            if(node.name == b->string()) {
                LOG(INFO) << "Reading number of bytes: " << size << " offset: " << offset;
                auto result = provider->read(node.name, buf, size, offset);
                LOG(INFO) << "Read bytes " << result;
                return result;
            }
        }
    }
    return 0;
}

void IFuse::registerFsProvider(VirtualFSProvider* provider) {
    auto p = std::make_pair<>(provider->getName(),  provider);
    fsProviders.insert(p);
}

void IFuse::removeFsProvider(std::string id) {
    fsProviders.erase(id);
}
