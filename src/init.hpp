#ifndef __INIT_H__
#define __INIT_H__
#include <opencv2/opencv.hpp>
#include <mutex>
#include <vector>
#include <string>
#include <deque>
#include <thread>

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

// flag
extern bool parsing_flag;

// mtx handle
extern std::mutex gMtxVideo;
extern std::mutex gMtxPre;
extern std::mutex gMtxInfer;

// video struct
extern std::vector<std::string> mVideoLists;
extern std::shared_ptr<std::deque<video_t>> VideoQueue;
extern std::shared_ptr<std::deque<video_t>> PreQueue;
extern std::shared_ptr<std::deque<video_t>> InferQueue;

#endif