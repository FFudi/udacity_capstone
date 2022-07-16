#include <iostream>
#include <string>
#include <thread>
#include "main.hpp"

// model init -> video parsing -> preprocess -> model running -> result rendering
int main()
{
    std::cout << " == Start Main Service == " << std::endl;

    // 1. model init && wait for preprocess image
    Model model("../model");

    // 2. preprocess init
    PreProcess preTask;

    // 3. parsing
    Utils utilsTask("../data", 224, 224);

	return 0;
}
