# model reference
# https://github.com/onnx/models/tree/main/vision/style_transfer/fast_neural_style

# onnx install
# https://stackoverflow.com/questions/63420533/setting-up-onnx-runtime-on-ubuntu-20-04-c-api
1. git clone --recursive https://github.com/Microsoft/onnxruntime
2. cd onnxruntime && ./build.sh --config RelWithDebInfo --build_wheel --update --build