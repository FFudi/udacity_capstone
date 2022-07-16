#ifndef __MODEL_H__
#define __MODEL_H__

#include <boost/filesystem.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <cstdio>
#include <cmath>
#include <numeric>
#include "utils.hpp"

template<typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T> &vec)
{
    os<<"[";
    for (int i = 0; i < vec.size(); i++)
    {
        os << vec.at(i);
        if(i != vec.size()-1) os <<", ";
    }
    os<<"]";
    return os;
}

template <typename T>
T vectorProduct(const std::vector<T>& v)
{
    return accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
}

std::string string_split(std::string str, char Delimiter);

class Onnx
{
public:
    Onnx(std::string model_path)
    {
        auto env_=std::make_unique<Ort::Env>(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING,
                                   string_split(model_path, '/').c_str());
        env = std::move(env_);
        
        auto session_ = std::make_unique<Ort::Session>(*env, model_path.c_str(), sessionOptions);
        session = std::move(session_);

        sessionOptions.SetIntraOpNumThreads(1);
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    }

    std::unique_ptr<Ort::Session> session = nullptr;
    std::unique_ptr<Ort::Env> env = nullptr;
    Ort::SessionOptions sessionOptions; 
    Ort::AllocatorWithDefaultOptions allocator;
};

struct currentModel_t
{
    int model_idx;
    std::string model_name;
    std::string model_path;
};

class Model
{
public:
    Model(std::string models_path)
    {
        _mRun(models_path);
    }
    retResult readModelList(std::string models_path);
    retResult Init(std::string models_path);
    void inference(void);
    void onnxExcute(const currentModel_t &currentModel, image_t &preImage, image_t &inferImage, image_t &resultImage);
    void _mRun(std::string models_path);
    void printInfo(const Ort::Session &session, const Ort::Allocator *allocator);
    currentModel_t modelSelect(int key_);


private : std::map<int, std::string> _mModelMapping;
    std::deque<Onnx> _ModelBank;
    currentModel_t currentModel = {0, "origin_name", "origin_path"};
    std::shared_ptr<float *> floatArr = std::make_shared<float *>();
};


#endif // __MODEL_H__