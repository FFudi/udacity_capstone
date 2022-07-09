#ifndef __MODEL_H__
#define __MODEL_H__

#include <boost/filesystem.hpp>
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
        readModelList(models_path);
        // Run();
    }
    retResult readModelList(std::string models_path);
    void Run(void);


private:
    std::map<int, std::string> _mModelMapping;
};


#endif // __MODEL_H__