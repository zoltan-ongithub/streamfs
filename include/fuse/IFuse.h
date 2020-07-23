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
    typedef std::string plugin_id;
    typedef std::string path_id;

    typedef std::pair<plugin_id , path_id> plugin_file_pair_t;
    typedef std::map<plugin_file_pair_t, struct fuse_pollhandle* > pollHandleMapType;


    static IFuse& getInstance()
    {
        static IFuse instance;
        return instance;
    }

    IFuse(IFuse const &) = delete;
    void operator=(IFuse const &x) = delete;
    void registerFsProvider(VirtualFSProvider* provider) override ;
    void removeFsProvider(std::string id) override;

    typedef  std::map<std::string, VirtualFSProvider*> fsProviderMapType;

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

    static int writeFileCallback(
            const char *path,
            const char * buff,
            size_t bufSize,
            off_t offset,
            struct fuse_file_info *fi);


    static VirtualFSProvider *findProvider(const char* path);

    static int poll(const char *, fuse_file_info *, fuse_pollhandle *, unsigned int *);

private:
    typedef struct fuse_pollhandle* pollHandleType;

    static void registerPoll(std::string filename, VirtualFSProvider *pProvider, struct fuse_pollhandle *ph);
};


#endif //STREAMLINK_IFUSE_H
