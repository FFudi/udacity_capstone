cmake_minimum_required(VERSION 3.0.0)

project(capstone VERSION 0.1.0)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_PREFIX_PATH /usr/local/lib ./onnx/lib)

add_executable(capstone ./src/main.cpp
./src/init.cpp
./src/preprocess.cpp
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

# onnxruntime
# find_library(ONNX_RUNTIME_LIB NAMES libonnxruntime.so HINTS ./onnx/lib)
find_library(ONNX_RUNTIME_LIB NAMES libonnxruntime.so HINTS /usr/local/lib)
include_directories(./onnx/include)
target_link_libraries(capstone ${ONNX_RUNTIME_LIB})
