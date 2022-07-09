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

void Model::Run(void)
{


}