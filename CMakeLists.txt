cmake_minimum_required(VERSION 3.0.0)
project(capstone VERSION 0.1.0)

add_executable(capstone ./src/main.cpp
./src/init.cpp
./src/utils.cpp
./src/model.cpp)

# boost
find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories(include ${Boost_INCLUDE_DIRS})
target_link_libraries(capstone ${Boost_FILESYSTEM_LIBRARY})

# opencv
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIR})
target_link_libraries(capstone ${OpenCV_LIBRARIES})

# thread
find_package(Threads REQUIRED)
target_link_libraries(capstone ${CMAKE_THREAD_LIBS_INIT})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
