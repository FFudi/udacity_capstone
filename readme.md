# model reference
# https://github.com/onnx/models/tree/main/vision/style_transfer/fast_neural_style

# 1. onnx install
# https://stackoverflow.com/questions/63420533/setting-up-onnx-runtime-on-ubuntu-20-04-c-api
1-1. git clone --recursive https://github.com/Microsoft/onnxruntime
1-2. cd onnxruntime && ./build.sh --config RelWithDebInfo --build_wheel --update --build

# 2. build project
2-1. cd ../ && sudo /sbin/ldconfig -v
2-2. mkdir build && cd build
2-3. cmake .. && make -j $(nproc)

# 3. execute project
3-1. ./capstone