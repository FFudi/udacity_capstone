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

class Model
{
public:
    Model(std::string models_path)
    {
        Init(models_path);
        // Run();
    }
    retResult readModelList(std::string models_path);
    retResult Init(std::string models_path);
    retResult Run(std::string models_path);

private:
    std::map<int, std::string> _mModelMapping;
    // std::map<int, std::string> _mModelMapping;
};


#endif // __MODEL_H__