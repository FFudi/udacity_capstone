#include <iostream>
#include <vector>
#include <string>
#include "preprocess.hpp"

void PreProcess::pre(void)
{
    int key;
    int videoCnt = 0;
    while(1)
    {
        // Todo : preprocess
        if (VideoQueue->size() != 0)
        {
            video_t *video = &(VideoQueue->front());

            // push result_pre to PreQueue
            gMtxPre.lock();

            video_t result_pre;
            result_pre.fileName = video->fileName;
            result_pre.totalFrame = video->totalFrame;
            result_pre.delay = video->delay;
            PreQueue->push_back(result_pre);

            gMtxPre.unlock();

            int frameCnt = 0;
            while (frameCnt < video->totalFrame)
            {
                if ((video->imageQueue)->size() != 0)
                {
                    gMtxVideo.lock();

                    image_t *image = &(video->imageQueue)->front();

                    std::cout << "PreProcess: " << video->fileName << "(" << image->width << "X" << image->height
                              << "), FPS: " << (int)(1000 / video->delay) << " [#" << image->framenumber << "]| " << std::endl;

                    // push to InferQueue
                    gMtxPre.lock();
                    // Todo : preprocess
                    // preImage


                    // ���⼭���� ����
                    // 1. onnx�� �� �� �ֵ��� ����ü ���⼭ �ϴ� ������ ��


                    // (PreQueue->back()).imageQueue->push_back(preImage);
                    gMtxPre.unlock();

                    (video->imageQueue)->pop_front();

                    gMtxVideo.unlock();
                    frameCnt++;
                }
            }
            VideoQueue->pop_front();
            videoCnt++;

            if (videoCnt == mVideoLists.size())
            {
                std::cout << "@@ PreProcess Done" << std::endl;
                return;
            }
        }
    }
}

void PreProcess::Run(void)
{
    std::thread preprocess_thrd(&PreProcess::pre, this);
    preprocess_thrd.detach();
}