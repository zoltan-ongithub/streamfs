//
// Created by Zoltan Kuscsik on 4/24/20.
//

#ifndef STREAMLINK_IFUSE_H
#define STREAMLINK_IFUSE_H
#define FUSE_USE_VERSION 26

extern "C" {
#include <fuse.h>
}

#include <string>
#include <map>
#include <streamfs/VirtualFSProvider.h>
#include <map>


class IFuse : public FileInterface {

protected:
    ~IFuse() = default;

public:
    /**
     * Plugin identifier type
     */
    typedef std::string plugin_id;
    /**
     * Path identifier type
     */
    typedef std::string path_id;
    /**
     * Fuse client context id
     */
    typedef std::string ctx_id_t;
    /**
     * Key type for identifying poll callbacks
     */
    typedef std::pair<plugin_id, path_id> plugin_file_pair_t;
    /**
     * Key type for managing callbacks on a client context
     */
    typedef std::map<IFuse::ctx_id_t, fuse_pollhandle *> contextMapType;
    /**
     * Map of fail pairs to set of contexts.
     */
    typedef std::map<plugin_file_pair_t, contextMapType> pollHandleMapType;

    static ctx_id_t fuseContextToId(fuse_context *ctx) {
        return std::to_string(ctx->gid) + "_" + std::to_string(ctx->pid);
    }

    static plugin_file_pair_t getCbId(plugin_id p, path_id path) {
        return std::make_pair(p, path);
    }

    static IFuse &getInstance() {
        static IFuse instance;
        return instance;
    }

    IFuse(IFuse const &) = delete;

    void operator=(IFuse const &x) = delete;

    void registerFsProvider(VirtualFSProvider *provider) override;

    void removeFsProvider(std::string id) override;

    typedef std::map<std::string, VirtualFSProvider *> fsProviderMapType;

    fuse_operations getFuseOperations();

    static void notifyPoll(IFuse::plugin_id provider, IFuse::path_id filename);

private:
    IFuse();

private:
    static std::map<std::string, std::string> mBaseDirs;
    static fsProviderMapType fsProviders;

private:
    // Fuse operations
    static int getAttrCallback(const char *path, struct stat *stbuf);

    static int readDirCallback(const char *path, void *buf, fuse_fill_dir_t filler,
                               off_t offset, struct fuse_file_info *fi);

    static int openCallback(const char *path, struct fuse_file_info *fi);

    static int readCallback(const char *path, char *buf, size_t size, off_t offset,
                            struct fuse_file_info *fi);

    static int truncate(const char *path, off_t lenght);
    static int release(const char *, fuse_file_info *);


    static int writeFileCallback(
            const char *path,
            const char *buff,
            size_t bufSize,
            off_t offset,
            struct fuse_file_info *fi);


    static VirtualFSProvider *findProvider(const char *path);

    static int poll(const char *, fuse_file_info *, struct fuse_pollhandle *, unsigned int *);

private:
    typedef struct fuse_pollhandle *pollHandleType;

    static std::mutex mStateMtx;

    static void
    registerPoll(std::string fileName, VirtualFSProvider *pProvider, struct fuse_pollhandle *ph, unsigned *reventsp);

};


#endif //STREAMLINK_IFUSE_H
