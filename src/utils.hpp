#ifndef __UTILS_H__
#define __UTILS_H__

#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "init.hpp"

enum class retResult
{
    ERROR = 0,
    SUCCESS
};

struct image_t
{
    std::string fileName;
    int totalFrame;
    int framenumber;
    int width;
    int height;
    cv::Mat frame;
};

struct video_t
{
    std::string fileName;
    int totalFrame;
    int delay;
    std::shared_ptr<std::deque<image_t>> imageQueue = std::make_shared<std::deque<image_t>>(); 
};

class Utils
{
public:
    Utils(std::string data_path, int width, int height)
    {
        parsing("../data", width, height);
    }
    retResult readFolderList(std::string data_path);
    retResult readVideoFile(std::string data_path, int set_width, int set_height);
    void parsing(std::string data_path, int set_width, int set_height);
    void rendering(void);

private:
    std::vector<std::string> mVideoLists;
    std::shared_ptr<std::deque<video_t>> mVideoQueue = std::make_shared<std::deque<video_t>>(); 
};



#endif