#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int deviceId) {
    cap.open(deviceId);
    if (!cap.isOpened()) {
        std::cerr << "Помилка: неможливо відкрити камеру!" << std::endl;
        exit(1);
    }
}

CameraProvider::~CameraProvider() { cap.release(); }

bool CameraProvider::getFrame(cv::Mat& frame) {
    cap >> frame;
    return !frame.empty();
}