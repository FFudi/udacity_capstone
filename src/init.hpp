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

// mtx handle
extern std::mutex gMtxVideo;
extern std::mutex gMtxPre;
extern std::mutex gMtxInfer;
extern std::mutex gMtxPost;

// video struct
extern std::vector<std::string> mVideoLists;
extern std::shared_ptr<std::deque<video_t>> VideoQueue;
extern std::shared_ptr<std::deque<video_t>> PreQueue;
extern std::shared_ptr<std::deque<video_t>> InferQueue;
extern std::shared_ptr<std::deque<video_t>> PostQueue;

class Init
{
// protected:
//     std::vector<std::string> mVideoLists;
//     std::shared_ptr<std::deque<video_t>> VideoQueue = std::make_shared<std::deque<video_t>>();
};

#endif