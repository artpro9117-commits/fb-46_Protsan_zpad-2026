#include "Display.hpp"
#include "FrameProcessor.hpp"

Display::Display(const std::string& name) : windowName(name) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    createUI();
}

void Display::show(const cv::Mat& frame) {
    cv::imshow(windowName, frame); // Відображення [cite: 253]
}

void Display::createUI() {
    // Створення слайдера для яскравості [cite: 252]
    FrameProcessor::brightness = 50; 
    cv::createTrackbar("Brightness", windowName, &FrameProcessor::brightness, 100);
}