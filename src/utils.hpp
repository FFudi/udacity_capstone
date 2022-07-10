#ifndef __UTILS_H__
#define __UTILS_H__

#include <boost/filesystem.hpp>
#include "init.hpp"

class Utils : public Init
{
public:
    Utils(std::string data_path, int width, int height)
    {
        Run("../data", width, height);
    }
    retResult readFolderList(std::string data_path);
    retResult readVideoFile(std::string data_path, int set_width, int set_height);
    void Run(std::string data_path, int set_width, int set_height);
    void rendering(void);

// private:
//     std::vector<std::string> mVideoLists;
//     std::shared_ptr<std::deque<video_t>> VideoQueue = std::make_shared<std::deque<video_t>>();
};



#endif