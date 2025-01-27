#include <iostream>
#include "model.hpp"

namespace fs = boost::filesystem;

std::string string_split(std::string str, char Delimiter)
{
    std::istringstream iss(str);
    std::string buffer;

    while (getline(iss, buffer, Delimiter)){}

    return buffer;
}

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

    for (int idx = 1; idx <= _mModelMapping.size(); idx++)
    {
        auto model_path = _mModelMapping.at(idx);
        printf("_mModelMapping.at(%d): %s\n", idx, model_path.c_str());

        Onnx ele_onnx(model_path);

        // In and Out nodes count
        size_t numInputNodes = ele_onnx.session->GetInputCount();
        size_t numOutputNodes = ele_onnx.session->GetOutputCount();

        std::cout << "Number of Input Nodes: " << numInputNodes << std::endl;
        std::cout << "Number of Output Nodes: " << numOutputNodes << std::endl;

        const char *inputName = ele_onnx.session->GetInputName(0, ele_onnx.allocator);
        std::cout << "Input Name: " << inputName << ", ";

        Ort::TypeInfo inputTypeInfo = ele_onnx.session->GetInputTypeInfo(0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

        std::vector<int64_t> inputDims = inputTensorInfo.GetShape();
        std::cout << "Input Dimensions: " << inputDims << std::endl;

        const char *outputName = ele_onnx.session->GetOutputName(0, ele_onnx.allocator);
        std::cout << "Output Name: " << outputName << ", ";

        Ort::TypeInfo outputTypeInfo = ele_onnx.session->GetOutputTypeInfo(0);
        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();

        std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
        std::cout << "Output Dimensions: " << outputDims << std::endl;

        _ModelBank.push_back(std::move(ele_onnx));
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

                        key = cv::waitKey(video->delay);

                        currentModel = this->modelSelect(key);

                        // excute model
                        onnxExcute(currentModel, preImage, inferImage, resultImage);

                        cv::imshow("Style transfer Viewer", resultImage.frame);

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

void Model::onnxExcute(const currentModel_t &currentModel, image_t &preImage, image_t &inferImage, image_t &resultImage)
{

    if (currentModel.model_name=="origin_name")
    {
        resultImage.frame = preImage.frame;
        return;
    }

    // load model by modelBank
    Onnx* model = &_ModelBank.at(currentModel.model_idx-1);

    // get INput, OUTput Info
    Ort::TypeInfo inputTypeInfo = model->session->GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

    std::vector<int64_t> inputDims = inputTensorInfo.GetShape();
    size_t inputTensorSize = vectorProduct(inputDims);
    std::vector<float> inputTensorValues(inputTensorSize);

    inputTensorValues.assign(inferImage.frame.begin<float>(),
                             inferImage.frame.end<float>());

    Ort::TypeInfo outputTypeInfo = model->session->GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();

    std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
    size_t outputTensorSize = vectorProduct(outputDims);
    std::vector<float> outputTensorValues(outputTensorSize);

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

    std::vector<const char *> inputNames{model->session->GetInputName(0, model->allocator)};
    std::vector<const char *> outputNames{model->session->GetOutputName(0, model->allocator)};

    // run
    auto output_tensors = model->session->Run(Ort::RunOptions{nullptr}, inputNames.data(),
                                      inputTensors.data(), 1, outputNames.data(), 1);

    printf("output_tensors.size(): %ld\n", output_tensors.size());
    float *floatArr = output_tensors.front().GetTensorMutableData<float>();

    cv::Mat1f result_1f = cv::Mat1f(224, 224, floatArr);
    result_1f.convertTo(resultImage.frame, CV_8U);
}

currentModel_t Model::modelSelect(int key_)
{
    std::string model_path;
    std::string model_name;
    std::map<const char *, const char *> modelInfo;

    switch (key_)
    {
    case '1':
        model_path = _mModelMapping.at(1);
        model_name = string_split(model_path, '/');
        currentModel = {1, model_name, model_path};
        printf("@@@@@@@@ onnx Model: %s\n", model_name.c_str());

        break;

    case '2':
        model_path = _mModelMapping.at(2);
        model_name = string_split(model_path, '/');
        currentModel = {2, model_name, model_path};
        printf("@@@@@@@@ onnx Model: %s\n", model_name.c_str());

        break;

    case '3':
        model_path = _mModelMapping.at(3);
        model_name = string_split(model_path, '/');
        currentModel = {3, model_name, model_path};
        printf("@@@@@@@@ onnx Model: %s\n", model_name.c_str());

        break;

    case '4':
        model_path = _mModelMapping.at(4);
        model_name = string_split(model_path, '/');
        currentModel = {4, model_name, model_path};
        printf("@@@@@@@@ onnx Model: %s\n", model_name.c_str());

        break;

    case '5':
        model_path = _mModelMapping.at(5);
        model_name = string_split(model_path, '/');
        currentModel = {5, model_name, model_path};
        printf("@@@@@@@@ onnx Model: %s\n", model_name.c_str());

        break;

    case 0x1B:
    case 'q':
    case 'Q': // ESC key, q, Q
        printf("--------------- quit program ---------------\n");
        exit(1);
        break;

    case '0':
        printf("Transfer Model execution stopped \n");
        currentModel = {0, std::string("origin_name"), std::string("origin_path")};
        break;

    default:
        break;
    }
    return currentModel;
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

