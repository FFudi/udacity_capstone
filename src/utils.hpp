#ifndef __UTILS_H__
#define __UTILS_H__

#include <boost/filesystem.hpp>
#include "init.hpp"

class Utils
{
public:
    Utils(std::string data_path, int width, int height)
    {
        _Run("../data", width, height);
    }
    retResult readFolderList(std::string data_path);
    retResult readVideoFile(std::string data_path, int set_width, int set_height);
    void _Run(std::string data_path, int set_width, int set_height);
    void rendering(void);
};

#endif