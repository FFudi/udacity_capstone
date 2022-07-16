# Monochrome Style Transfer
This capstone project converts the videos in the data folder to the desired monochrome style.

# 1. onnx setting and build project
1-1. mkdir build && cd build
1-2. cmake .. && make -j $(nproc)
1-3. cd ../ && sudo ldconfig

# 3. execute project
3-1. cd build && ./capstone

# 4. select model (in model folder)
if you select a number key from 1 to 5 in the image viewer, you can check the corresponding converted image.
You can also quit the project by pressing the q or esc key.
(Selecting 0 key returns to the original image.)
-->[0] origin image
-->[1] candy.onnx style
-->[2] mosaic.onnx style
-->[3] rain-princess.onnx style
-->[4] udnie.onnx style
-->[5] pointilism.onnx style
-->[q, Q, ESC key] quit program

# onnx model reference
# https://github.com/onnx/models/tree/main/vision/style_transfer/fast_neural_style
