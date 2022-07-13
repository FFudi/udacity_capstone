#include <iostream>
#include "model.hpp"

namespace fs = boost::filesystem;

retResult Model::readModelList(std::string models_path)
{
    retResult ret = retResult::ERROR;
    
    int i = 1;
    for (const auto &entry : fs::directory_iterator(models_path))
    {
        _mModelMapping.insert(std::pair<int, std::string>(i, entry.path().string()));
        i++;
    }

    // check empty folder
    if (_mModelMapping.size() == 0)
    {
        std::cout << "model folder is empty. " << std::endl;
        return ret;
    }

    std::cout << "@ total model count is " << _mModelMapping.size() << std::endl;
    for (std::map<int, std::string>::iterator it = _mModelMapping.begin(); it != _mModelMapping.end(); it++)
    {
        printf("--> [%d] %s\n", it->first, (it->second).c_str());
    }

    ret = retResult::SUCCESS;
    return ret;
}

retResult Model::Init(std::string models_path)
{
    auto ret = retResult::ERROR;
    if(readModelList(models_path)!=retResult::SUCCESS) return ret;

    // Todo : range_for() about 5 onnx model
    // for(int idx= 0; idx<_mModelMapping.size(); idx++)
    for (int idx = 0; idx < 1; idx++)
    {
        // std::string temp_model = "candy";
        auto model_path = _mModelMapping.at(1);
        printf("_mModelMapping.at(0).c_str(): %s\n", model_path.c_str());

        // loading network
        std::string temp_model = "candy";

        // onnx initialize
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(1);
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // define onnx session
        Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, temp_model.c_str());
        Ort::Session session(env, model_path.c_str(), sessionOptions);

        Ort::AllocatorWithDefaultOptions allocator;

        // printInfo(session, &allocator);
        // In and Out nodes count
        size_t numInputNodes = session.GetInputCount();
        size_t numOutputNodes = session.GetOutputCount();

        std::cout << "Number of Input Nodes: " << numInputNodes << std::endl;
        std::cout << "Number of Output Nodes: " << numOutputNodes << std::endl;

        const char *inputName = session.GetInputName(0, allocator);
        std::cout << "Input Name: " << inputName << ", ";

        Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

        std::vector<int64_t> inputDims = inputTensorInfo.GetShape();
        std::cout << "Input Dimensions: " << inputDims << std::endl;

        const char *outputName = session.GetOutputName(0, allocator);
        std::cout << "Output Name: " << outputName << ", ";

        Ort::TypeInfo outputTypeInfo = session.GetOutputTypeInfo(0);
        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();

        std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
        std::cout << "Output Dimensions: " << outputDims << std::endl;

        // onnx_t _mOnnx{std::move(sessionOptions), std::move(env), std::move(session), std::move(allocator)};
        // _ModelBank.insert({0, std::move(_mOnnx)});
    }

    ret = retResult::SUCCESS;
    return ret;
}

void Model::inference(void)
{
    int key;
    int videoCnt = 0;
    int frameNum = 1;

    while (1)
    {
        gMtxPre.lock();
        if ((parsing_flag == true) && (videoCnt == mVideoLists.size()))
        {

            std::cout << "@@ inference Done" << std::endl;
            exit(1);
            return;
        }
        else
        {
            gMtxInfer.lock();
            if (PreQueue->size() != 0)
            {
                video_t *video = &(PreQueue->front());

                if (InferQueue->size() == 0)
                {
                    video_t result_infer;
                    result_infer.fileName = video->fileName;
                    result_infer.totalFrame = video->totalFrame;
                    result_infer.delay = video->delay;
                    InferQueue->push_back(result_infer);
                }
                else
                {
                    while ((video->imageQueue)->size() != 0)
                    {
                        image_t inferImage, resultImage;
                        image_t preImage = (video->imageQueue)->front();
                        (video->imageQueue)->pop_front();

                        std::cout << "[#" << preImage.framenumber << "] "
                                  << "Inference Stage: " << video->fileName << "(" << preImage.width << "X" << preImage.height
                                  << "), FPS: " << (int)(1000 / video->delay) << std::endl;

                        cv::dnn::blobFromImage(preImage.frame, inferImage.frame);


                        // inference
                        // select model
                        // onnx_t model = std::move(_ModelBank.at(0));
                        // std::string temp_model = "candy";

                        auto model_path = _mModelMapping.at(2);
                        printf("_mModelMapping.at(0).c_str(): %s\n", model_path.c_str());

                        // loading network
                        std::string temp_model = "candy";

                        // onnx initialize
                        Ort::SessionOptions sessionOptions;
                        sessionOptions.SetIntraOpNumThreads(1);
                        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

                        // define onnx session
                        Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, temp_model.c_str());
                        Ort::Session session(env, model_path.c_str(), sessionOptions);

                        Ort::AllocatorWithDefaultOptions allocator;
                        ////////////////////////////////
                        Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
                        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

                        std::vector<int64_t> inputDims = inputTensorInfo.GetShape();
                        size_t inputTensorSize = vectorProduct(inputDims);
                        std::vector<float> inputTensorValues(inputTensorSize);

                        inputTensorValues.assign(inferImage.frame.begin<float>(),
                                                 inferImage.frame.end<float>());

                        Ort::TypeInfo outputTypeInfo = session.GetOutputTypeInfo(0);
                        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();

                        std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
                        size_t outputTensorSize = vectorProduct(outputDims);
                        std::vector<float> outputTensorValues(outputTensorSize);

                        // ////////////////////////////////
                        std::vector<Ort::Value> inputTensors;
                        std::vector<Ort::Value> outputTensors;
                        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator,
                                                                                OrtMemType::OrtMemTypeDefault);

                        inputTensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                                               inputTensorSize, inputDims.data(),
                                                                               inputDims.size()));

                        outputTensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo,
                                                                                outputTensorValues.data(),
                                                                                outputTensorSize,
                                                                                outputDims.data(),
                                                                                outputDims.size()));
                       
                        // ////////////////////////////////
                        std::vector<const char *> inputNames{session.GetInputName(0, allocator)};
                        std::vector<const char *> outputNames{session.GetOutputName(0, allocator)};

                        // session.Run(Ort::RunOptions{nullptr}, inputNames.data(),
                        //             inputTensors.data(), 1, outputNames.data(),
                        //             outputTensors.data(), 1);
                        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, inputNames.data(),
                                                          inputTensors.data(), 1, outputNames.data(), 1);
                        
                        printf("output_tensors.size(): %ld\n", output_tensors.size());
                        float *floatarr = output_tensors.front().GetTensorMutableData<float>();
                        cv::Mat1f result_1f = cv::Mat1f(224, 224, floatarr);
                        // result_1f *= 255;
                        result_1f.convertTo(resultImage.frame, CV_8U);
                        // ////////////////////////////////


                        // validation code
                        cv::imshow("Style transfer Viewer", resultImage.frame);
                        key = cv::waitKey(video->delay);
                        if (key == 'q')
                            exit(1);

                        (InferQueue->back()).imageQueue->push_back(inferImage);
                        frameNum++;
                    }

                    if (frameNum == video->totalFrame)
                    {
                        PreQueue->pop_front();
                        videoCnt++;
                        frameNum = 1;
                    }
                }
            }
            gMtxInfer.unlock();
        }
        gMtxPre.unlock();
    }
}


void Model::_mRun(std::string models_path)
{
    if(Init(models_path)!=retResult::SUCCESS)
    {
        std::cout << "model loading fail." <<std::endl;
        exit(1);
    }

    std::thread inference_thrd(&Model::inference, this);
    inference_thrd.detach();
}

