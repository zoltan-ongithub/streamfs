//
// Created by Zoltan Kuscsik on 4/17/20.
//

#ifndef STREAMFS_PLUGINIMPL_H
#define STREAMFS_PLUGINIMPL_H
#include <string>

namespace streamfs {
class PluginImpl {
public:
    int load(std::string path);
};
}

#endif //STREAMFS_PLUGINIMPL_H
