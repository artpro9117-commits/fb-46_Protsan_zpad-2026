#include "FrameProcessor.hpp"
#include <vector>

int FrameProcessor::brightness = 0; // Ініціалізація статичної змінної

void FrameProcessor::process(cv::Mat& input, cv::Mat& output, Mode mode, const std::vector<cv::Rect>& faces) {
    input.convertTo(output, -1, 1, brightness - 50);

    switch (mode) {
        case Mode::NORMAL: 
            break;
        case Mode::INVERT: 
            cv::bitwise_not(output, output); // Інверсія [cite: 237]
            break;
        case Mode::BLUR: 
            cv::GaussianBlur(output, output, cv::Size(15, 15), 0); // Gaussian blur [cite: 238]
            break;
        case Mode::CANNY: 
            cv::cvtColor(output, output, cv::COLOR_BGR2GRAY);
            cv::Canny(output, output, 50, 150); // Canny фільтр [cite: 239]
            break;
        case Mode::GLITCH: 
            applyGlitch(output); // Розсування каналів [cite: 245]
            break;
        case Mode::FACE:
            for (const auto& rect : faces) {
                cv::rectangle(output, rect, cv::Scalar(0, 255, 0), 2);
                cv::putText(output, "Face", cv::Point(rect.x, rect.y - 10), 
                            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            }
            break;
    }
    
    // Додаємо лічильник FPS або текст [cite: 249]
    cv::putText(output, "Mode: " + std::to_string(static_cast<int>(mode)), 
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
}

void FrameProcessor::applyGlitch(cv::Mat& img) {
    if(img.channels() != 3) return;
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    
    // Зсуваємо червоний канал вправо, синій вліво
    cv::Mat shiftR = cv::Mat::zeros(channels[2].size(), channels[2].type());
    cv::Mat shiftB = cv::Mat::zeros(channels[0].size(), channels[0].type());
    
    channels[2](cv::Rect(0, 0, img.cols - 10, img.rows)).copyTo(shiftR(cv::Rect(10, 0, img.cols - 10, img.rows)));
    channels[0](cv::Rect(10, 0, img.cols - 10, img.rows)).copyTo(shiftB(cv::Rect(0, 0, img.cols - 10, img.rows)));
    
    channels[2] = shiftR;
    channels[0] = shiftB;
    cv::merge(channels, img);
}