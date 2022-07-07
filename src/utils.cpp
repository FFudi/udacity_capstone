#include <iostream>
#include "utils.hpp"

namespace fs = boost::filesystem;

retResult Utils::readFolderList(std::string data_path)
{
    retResult ret = retResult::ERROR;
    for (const auto &entry : fs::directory_iterator(data_path))
    {
        mVideoLists.push_back(entry.path().string());
    }

    // check empty folder
    if (mVideoLists.size() == 0)
    {
        std::cout << "data folder is empty. " << std::endl;
        return ret;
    }

    std::cout << "@ total file count is " << mVideoLists.size() << std::endl;
    for (const auto &ele : mVideoLists)
    {
        std::cout << "[" << &ele - &*mVideoLists.begin() << "]: " << ele << std::endl;
    }
    ret = retResult::SUCCESS;
    return ret;
}

retResult Utils::readVideoFile(std::string data_path, int set_width, int set_height)
{
    retResult ret = this->readFolderList(data_path);

    // empty folder
    if (ret == retResult::ERROR)
        exit(1);

    // video capture
    for (auto &fileName : mVideoLists)
    {
        std::cout << "\nREAD FILE : " << fileName << std::endl;
        cv::VideoCapture capture(fileName);

        // check isopened video
        if (!capture.isOpened())
        {
            std::cout << "can't open this video: " << fileName << std::endl;
            exit(1);
        }

        // video info init
        cv::Mat frame;
        double frame_rate = capture.get(cv::CAP_PROP_FPS); // frame per second
        int delay = 1000 / frame_rate;
        auto totalFrame = capture.get(cv::CAP_PROP_FRAME_COUNT);
        int key = 0;

        // define video_t variable
        video_t video;
        video.fileName = fileName;
        video.totalFrame = totalFrame;
        video.delay = delay;
        mVideoQueue->push_back(video);

        // play video

        while (capture.read(frame))
        {
            mMtx.lock();

            int current = capture.get(cv::CAP_PROP_POS_FRAMES);

            // resize
            cv::resize(frame, frame, cv::Size(set_width, set_height));

            image_t image{fileName, (int)totalFrame, current, set_width, set_height, frame};
            (mVideoQueue->back()).imageQueue->push_back(image);

            mMtx.unlock();
        }
        capture.release();
    }

    ret = retResult::SUCCESS;
    return ret;
}

void Utils::parsing(std::string data_path, int set_width, int set_height)
{
    std::thread parsing_thrd(&Utils::readVideoFile, this, data_path, set_width, set_height);
    std::thread rendering_thrd(&Utils::rendering, this);
    parsing_thrd.join();
    rendering_thrd.join();
}

void Utils::rendering(void)
{
    int key;
    int videoCnt = 0;

    while (1)
    {
        if (mVideoQueue->size() != 0)
        {
            video_t *video = &(mVideoQueue->front());
            int delay = video->delay;
            int frameCnt = 0;
            while (frameCnt < video->totalFrame)
            {
                if ((video->imageQueue)->size() != 0)
                {
                    mMtx.lock();

                    image_t *image = &(video->imageQueue)->front();

                    std::cout << video->fileName << "(" << image->width << "X" << image->height << "), FPS: " << (int)(1000 / delay)
                              << " [#" << image->framenumber << "]| " << std::endl;

                    // draw image
                    std::string scene = std::string("[#") + std::to_string(image->framenumber) + std::string("]");
                    cv::putText(image->frame, video->fileName + scene, cv::Point(20, 40), cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 0), 2);

                    cv::imshow("Fire Detection Viewer", image->frame);
                    key = cv::waitKey(video->delay);
                    if (key == 'q')
                        exit(1);

                    (video->imageQueue)->pop_front();
                    frameCnt++;
                    
                    mMtx.unlock();
                }
            }
            mVideoQueue->pop_front();
            videoCnt++;
        }

        if (videoCnt == mVideoLists.size())
        {
            cv::destroyAllWindows();
            std::cout << "@@ complete fire detection" << std::endl;
            exit(1);
        }
    }
}
