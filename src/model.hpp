#ifndef __MODEL_H__
#define __MODEL_H__

#include <boost/filesystem.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <cstdio>
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

struct onnx_t
{
    Ort::SessionOptions &&sessionOptions;
    Ort::Env &&env;
    Ort::Session &&session;
    Ort::AllocatorWithDefaultOptions &&allocator;
};

class Model
{
public:
    Model(std::string models_path)
    {
        Run(models_path);
    }
    retResult readModelList(std::string models_path);
    retResult Init(std::string models_path);
    void inference(void);
    void Run(std::string models_path);
    void printInfo(const Ort::Session &session, const Ort::Allocator *allocator);


private:
    std::map<int, std::string> _mModelMapping;
    std::map<int, onnx_t> _ModelBank;
};


#endif // __MODEL_H__