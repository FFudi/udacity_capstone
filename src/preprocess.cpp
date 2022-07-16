#include <iostream>
#include <vector>
#include <string>
#include "preprocess.hpp"

void PreProcess::pre(void)
{
    int key;
    int videoCnt = 0;
    int frameNum = 1;

    while (1)
    {
        gMtxVideo.lock();
        if ((parsing_flag == true) && (videoCnt == mVideoLists.size()))
        {

            std::cout << "@@ PreProcess Done" << std::endl;
            // exit(1);
            return;
        }
        else
        {
            // gMtxPre.lock();
            if (VideoQueue->size() != 0)
            {
                video_t *video = &(VideoQueue->front());

                if (PreQueue->size() == 0)
                {
                    video_t result_pre;
                    result_pre.fileName = video->fileName;
                    result_pre.totalFrame = video->totalFrame;
                    result_pre.delay = video->delay;
                    PreQueue->push_back(result_pre);
                }
                else
                {
                    while ((video->imageQueue)->size() != 0)
                    {
                        image_t preImage = (video->imageQueue)->front();
                        (video->imageQueue)->pop_front();

                        std::cout << "[#" << preImage.framenumber << "] "
                                  << "PreProcess Stage: " << video->fileName << "(" << preImage.width << "X" << preImage.height
                                  << "), FPS: " << (int)(1000 / video->delay) << std::endl;

                        // preprocess
                        (preImage.frame).convertTo(preImage.frame, CV_32F, 1.0/255); // cvt CV_32F, 1.0/255

                        (PreQueue->back()).imageQueue->push_back(preImage);
                        frameNum++;
                    }

                    if (frameNum == video->totalFrame)
                    {
                        VideoQueue->pop_front();
                        videoCnt++;
                        frameNum = 1;
                    }
                }
            }
            // gMtxPre.unlock();
        }
        gMtxVideo.unlock();
    }
}

void PreProcess::Run(void)
{
    std::thread preprocess_thrd(&PreProcess::pre, this);
    preprocess_thrd.detach();
}