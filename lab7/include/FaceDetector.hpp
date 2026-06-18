#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxt, const std::string& model);
    ~FaceDetector();
    
    void setFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces();

private:
    void workerLoop();
    
    cv::dnn::Net net;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    
    std::atomic<bool> running;
    std::atomic<bool> hasNewFrame;
    
    cv::Mat sharedFrame;
    std::vector<cv::Rect> detectedFaces;
};