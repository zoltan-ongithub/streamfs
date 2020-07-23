//
// Created by Zoltan Kuscsik on 4/24/20.
//

#include <cstring>
#include "fuse/IFuse.h"
#include <boost/filesystem.hpp>
#include <string>
#include <poll.h>

#define fs boost::filesystem

static std::mutex mPollConfigMtx;

static IFuse::pollHandleMapType mPollHandles;
IFuse::fsProviderMapType IFuse::fsProviders;

static struct fuse *g_fsel_fuse;

IFuse::IFuse(){
}

fuse_operations IFuse::getFuseOperations() {
    static fuse_operations ops = []{
        fuse_operations fop{};
        fop.getattr = IFuse::getAttrCallback;
	fop.open = IFuse::openCallback;
	fop.read = IFuse::readCallback;
	fop.readdir = IFuse::readDirCallback;
	fop.write = IFuse::writeFileCallback;
	fop.truncate = IFuse::truncate;
	fop.poll = IFuse::poll;
	return fop;
    }();
  return ops;
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
                            stbuf->st_size = INT64_MAX;
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

    return 0;
}
static std::string findNode(VirtualFSProvider* provider, const char* fusePath) {
    fs::path p(fusePath);
    auto b = p.begin();
    std::advance(b, 2);
    if (provider != nullptr){
        auto fsNodes = provider->getNodes();
        for(auto node : fsNodes) {
            // TODO: provider should be use hash based lookup
            if(node.name == b->string()) {
                return node.name;
            }
        }
    }

    return "";
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

int IFuse::readCallback(const char *path, char *buf, size_t size, off_t offset,
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
                auto result = provider->read(node.name, buf, size, offset);
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

int IFuse::writeFileCallback(
        const char *path,
        const char *buf,
        size_t bufSize,
        off_t offset,
        struct fuse_file_info *) {


    auto provider = findProvider(path);
    auto node = findNode(provider, path);

    if (node.empty()) {
        LOG(INFO) << "Could not find node " << path;
        return bufSize;
    }

    return provider->write(node.c_str(), buf, bufSize, offset);
}

int IFuse::truncate(const char *path, off_t lenght) {
    return 0;
}

int IFuse::poll(const char *path, struct fuse_file_info *fi,
                struct fuse_pollhandle *ph, unsigned *reventsp) {

    auto provider = findProvider(path);
    auto node = findNode(provider, path);

    if (node.empty()) {
        LOG(INFO) << "Could not find node " << path;
        return 0;
    }

    std::lock_guard<std::mutex> lockGuard(mPollConfigMtx);

    if (!g_fsel_fuse) {
        struct fuse_context *cxt = fuse_get_context();
        if (cxt)
            g_fsel_fuse = cxt->fuse;
    }

    registerPoll(node, provider, ph);

    return 0;
}

void IFuse::registerPoll(std::string fileName, VirtualFSProvider *pProvider, struct fuse_pollhandle *ph)
{
    auto id = std::make_pair(pProvider->getName(), fileName);
    auto handle = mPollHandles.find(id);

    if (mPollHandles.find(id) != mPollHandles.end()) {
        fuse_pollhandle_destroy(handle->second);
    }

    auto phPair = std::make_pair(id, ph);
    mPollHandles.insert(phPair);
}

void IFuse::notifyPoll(IFuse::plugin_id provider,
                       IFuse::path_id filename) {
    std::lock_guard<std::mutex> lockGuard(mPollConfigMtx);
    auto id = std::make_pair(provider, filename);
    auto handle = mPollHandles.find(id);
    if (mPollHandles.find(id) != mPollHandles.end()) {
        auto res = fuse_notify_poll(handle->second);
        fuse_pollhandle_destroy(handle->second);
        mPollHandles.erase(id);
    }

}
