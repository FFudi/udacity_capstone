#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "main.hpp"

int main()
{
    std::cout << " == Start Main Service == " << std::endl;
    // run();
    Utils utils;
    utils.parsing("../data", 720, 480);



	return 0;
}
