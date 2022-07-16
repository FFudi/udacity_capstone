#include "init.hpp"

// flag
bool parsing_flag = false;

// mutex handle
std::mutex gMtxVideo;
std::mutex gMtxPre;
std::mutex gMtxInfer;

// video deque
std::vector<std::string> mVideoLists;
std::shared_ptr<std::deque<video_t>> VideoQueue = std::make_shared<std::deque<video_t>>();
std::shared_ptr<std::deque<video_t>> PreQueue = std::make_shared<std::deque<video_t>>();
std::shared_ptr<std::deque<video_t>> InferQueue = std::make_shared<std::deque<video_t>>();