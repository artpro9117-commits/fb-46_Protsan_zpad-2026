#!/bin/bash
echo "Встановлення залежностей для OpenCV та CMake..."
sudo apt update
sudo apt install -y libopencv-dev cmake g++ make
echo "Готово!"