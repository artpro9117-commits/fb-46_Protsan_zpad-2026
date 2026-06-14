#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

int main() {
    CameraProvider camera(0);
    KeyProcessor input;
    FrameProcessor processor;
    Display display("Lab 6 - OpenCV");

    cv::Mat frame, processedFrame;

    while (true) {
        if (!camera.getFrame(frame)) break;

        // Обробка кадру в залежності від поточного режиму
        processor.process(frame, processedFrame, input.getMode());

        // Вивід на екран
        display.show(processedFrame);

        // Обробка клавіатури [cite: 234]
        int key = cv::waitKey(30);
        if (key >= 0 && !input.processKey(key)) {
            break; // Вихід, якщо натиснуто ESC або q
        }
    }
    return 0;
}