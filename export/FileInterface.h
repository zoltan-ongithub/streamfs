//
// Created by Zoltan Kuscsik on 4/26/20.
//

#ifndef STREAMLINK_FILEINTERFACE_H
#define STREAMLINK_FILEINTERFACE_H
#include <VirtualFSProvider.h>

class VirtualFSProvider;

class FileInterface {
public:
    virtual void registerFsProvider(VirtualFSProvider* provider) = 0 ;
    virtual void removeFsProvider(std::string id)  = 0;
};

#endif //STREAMLINK_FILEINTERFACE_H
