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

// class Onnx
// {
// public:
//     Ort::SessionOptions sessionOptions;
//     Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "candy.onnx");
//     Ort::Session session(env, "../model/candy.onnx", sessionOptions);
//     Ort::AllocatorWithDefaultOptions allocator;

// };

struct currentModel_t
{
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
    std::string string_split(std::string str, char Delimiter);

private : std::map<int, std::string> _mModelMapping;
    // std::deque<Onnx> _ModelBank;
    currentModel_t currentModel = {"origin_name", "origin_path"};
    std::shared_ptr<float *> floatArr = std::make_shared<float *>();
};


#endif // __MODEL_H__