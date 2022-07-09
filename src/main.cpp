#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "main.hpp"

int main()
{
    std::cout << " == Start Main Service == " << std::endl;

    // model init -> video parsing -> preprocess -> model running -> postprocess -> result rendering

    // 1. model init
    Model model("../model");
    // 2. preprocess init

    // 3. model running

    // 4. postprocess 

    // 5. rendering
    Utils utils("../data", 720, 480);

	return 0;
}
