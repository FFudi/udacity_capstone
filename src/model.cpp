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

    // Todo : range_for()

    // loading network   
    std::string temp_model = "candy";
    auto temp_model_path = _mModelMapping.at(1);
    printf("_mModelMapping.at(0).c_str(): %s\n", temp_model_path.c_str());

    // onnx initialize
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // define onnx session
    Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, temp_model.c_str());
    Ort::Session session(env, temp_model_path.c_str(), sessionOptions);

    Ort::AllocatorWithDefaultOptions allocator;

    // printInfo(session, &allocator);
    // In and Out nodes count
    size_t numInputNodes = session.GetInputCount();
    size_t numOutputNodes = session.GetOutputCount();

    std::cout << "Number of Input Nodes: " << numInputNodes << std::endl;
    std::cout << "Number of Output Nodes: " << numOutputNodes << std::endl;

    const char* inputName = session.GetInputName(0, allocator);
    std::cout << "Input Name: " << inputName << ", ";

    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

    std::vector<int64_t> inputDims = inputTensorInfo.GetShape();
    std::cout << "Input Dimensions: " << inputDims << std::endl;

    const char* outputName = session.GetOutputName(0, allocator);
    std::cout << "Output Name: " << outputName << ", ";

    Ort::TypeInfo outputTypeInfo = session.GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    
    std::vector<int64_t> outputDims = outputTensorInfo.GetShape();
    std::cout << "Output Dimensions: " << outputDims << std::endl;

    onnx_t _mOnnx{std::move(sessionOptions), std::move(env), std::move(session), std::move(allocator)};
    // temp model
    // _ModelBank.insert({0, onnx_t{sessionOptions, env, session, allocator}});
    _ModelBank.insert({0, std::move(_mOnnx)});

    return ret;
}

void Model::inference(void)
{
    while(1)
    {
        
    }
}

void Model::Run(std::string models_path)
{
    if(Init(models_path)!=retResult::SUCCESS)
    {
        std::cout << "model loading fail." <<std::endl;
        exit(1);
    }

    std::thread inference_thrd(&Model::inference, this);
    inference_thrd.join();
    // cv::dnn::blobFromImage(imageRGB, imageRGB);
}

