#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    static int brightness; // Для слайдера
    void process(cv::Mat& input, cv::Mat& output, Mode mode, const std::vector<cv::Rect>& faces = {});
private:
    void applyGlitch(cv::Mat& img);
};