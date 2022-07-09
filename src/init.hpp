#ifndef __INIT_H__
#define __INIT_H__
#include <mutex>

extern std::mutex gMtxPre;
extern std::mutex gMtxInfer;
extern std::mutex gMtxPost;

#endif