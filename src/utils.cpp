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

    std::cout << "\n@ total video count is " << mVideoLists.size() << std::endl;
    for (const auto &ele : mVideoLists)
    {
        std::cout << "[" << &ele - &*mVideoLists.begin() << "]: " << ele << std::endl;
    }
    parsing_flag = true;
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
        gMtxVideo.lock();
        video_t video;
        video.fileName = fileName;
        video.totalFrame = totalFrame;
        video.delay = delay;
        VideoQueue->push_back(video);
        gMtxVideo.unlock();

        // play video
        int frameNum = 1;
        while (capture.read(frame))
        {
            gMtxVideo.lock();

            int current = capture.get(cv::CAP_PROP_POS_FRAMES);

            // resize
            cv::resize(frame, frame, cv::Size(set_width, set_height));
            printf("------------------------------------------------------------\n");
            printf("[#%d] Parsing Stage: \n", frameNum++);

            image_t image{fileName, (int)totalFrame, current, set_width, set_height, frame};
            (VideoQueue->back()).imageQueue->push_back(image);
            gMtxVideo.unlock();
        }
        capture.release();
    }

    ret = retResult::SUCCESS;
    return ret;
}

void Utils::_Run(std::string data_path, int set_width, int set_height)
{
    std::thread parsing_thrd(&Utils::readVideoFile, this, data_path, set_width, set_height);
    // std::thread rendering_thrd(&Utils::rendering, this);
    parsing_thrd.join();
    // rendering_thrd.join();
}

void Utils::rendering(void)
{
    int key;
    int videoCnt = 0;
    // cv::namedWindow("Style transfer Viewer",  cv::WindowFlags::WINDOW_NORMAL);
    cv::namedWindow("Style transfer Viewer2", cv::WindowFlags::WINDOW_KEEPRATIO);
    // cv::namedWindow("Style transfer Viewer");

    while (1)
    {
        if (VideoQueue->size() != 0)
        {
            video_t *video = &(VideoQueue->front());
            int delay = video->delay;
            int frameCnt = 0;
            while (frameCnt < video->totalFrame)
            {
                if ((video->imageQueue)->size() != 0)
                {
                    gMtxVideo.lock();
                    // gMtxPost.lock();

                    image_t *image = &(video->imageQueue)->front();

                    std::cout << video->fileName << "(" << image->width << "X" << image->height << "), FPS: " << (int)(1000 / delay)
                              << " [#" << image->framenumber << "]| " << std::endl;

                    // draw image
                    std::string scene = std::string("[#") + std::to_string(image->framenumber) + std::string("]");
                    cv::putText(image->frame, video->fileName + scene, cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1);

                    cv::imshow("Style transfer Viewer", image->frame);
                    key = cv::waitKey(video->delay);
                    if (key == 'q')
                        exit(1);

                    (video->imageQueue)->pop_front();
                    frameCnt++;
                    // gMtxPost.unlock();
                    gMtxVideo.unlock();
                }
            }
            VideoQueue->pop_front();
            videoCnt++;

            if (videoCnt == mVideoLists.size())
            {
                cv::destroyAllWindows();
                std::cout << "@@ complete capstone" << std::endl;
                exit(1);
            }
        }
    }
}
