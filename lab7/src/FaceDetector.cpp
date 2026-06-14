#include "FaceDetector.hpp"
#include <chrono>
#include <iostream>

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& model) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxt, model);
    } catch (const cv::Exception& e) {
        std::cerr << "Помилка завантаження мережі! Перевірте чи існують файли .prototxt та .caffemodel" << std::endl;
        exit(1);
    }
    
    running = true;
    hasNewFrame = false;
    // Запуск фонового потоку
    worker = std::thread(&FaceDetector::workerLoop, this);
}

FaceDetector::~FaceDetector() {
    running = false;
    cv.notify_one(); // Будимо потік, якщо він спав, щоб він міг завершитись
    if (worker.joinable()) {
        worker.join();
    }
}

void FaceDetector::setFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frame.copyTo(sharedFrame); // Копіюємо кадр для фонового потоку
    hasNewFrame = true;
    cv.notify_one(); // Сповіщаємо фоновий потік, що є робота
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(mtx);
    return detectedFaces; // Повертаємо останній відомий результат
}

void FaceDetector::workerLoop() {
    while (running) {
        cv::Mat frameToProcess;
        
        {
            std::unique_lock<std::mutex> lock(mtx);
            // Потік засинає, поки немає нового кадру або не надійшла команда зупинки
            cv.wait(lock, [this]() { return hasNewFrame.load() || !running.load(); });
            
            if (!running) break;
            
            sharedFrame.copyTo(frameToProcess);
            hasNewFrame = false;
        }

        if (frameToProcess.empty()) continue;

        // ШТУЧНЕ НАВАНТАЖЕННЯ: Затримка 500 мс для демонстрації роботи потоків
        // У цьому місці основне відео не зависне, але рамка буде оновлюватись раз на півсекунди
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Підготовка даних для нейромережі
        cv::Mat blob = cv::dnn::blobFromImage(frameToProcess, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
        net.setInput(blob);
        cv::Mat detections = net.forward();

        std::vector<cv::Rect> faces;
        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

        // Фільтрація облич з confidence > 50%
        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameToProcess.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameToProcess.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameToProcess.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameToProcess.rows);
                faces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }

        // Оновлюємо результати під м'ютексом
        {
            std::lock_guard<std::mutex> lock(mtx);
            detectedFaces = faces;
        }
    }
}