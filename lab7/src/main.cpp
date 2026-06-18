#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include "FaceDetector.hpp"

int main() {
    CameraProvider camera(0);
    KeyProcessor input;
    FrameProcessor processor;
    Display display("Lab 7 - MultiThreaded CV");

    // Ініціалізація детектора облич. Програма завантажить моделі з файлів.
    FaceDetector faceDetector("../deploy.prototxt", "../res10_300x300_ssd_iter_140000.caffemodel");

    cv::Mat frame, processedFrame;
    std::vector<cv::Rect> currentFaces;

    while (true) {
        if (!camera.getFrame(frame)) break;

        // Логіка багатопотоковості: 
        if (input.getMode() == Mode::FACE) {
            // 1. Відправляємо кадр у фоновий потік
            faceDetector.setFrame(frame);
            // 2. Забираємо ОСТАННІ готові координати (вони можуть бути від попереднього кадру)
            currentFaces = faceDetector.getFaces();
        } else {
            currentFaces.clear();
        }

        // Обробка та вивід
        processor.process(frame, processedFrame, input.getMode(), currentFaces);
        display.show(processedFrame);

        int key = cv::waitKey(30);
        if (key >= 0 && !input.processKey(key)) break;
    }
    return 0;
}