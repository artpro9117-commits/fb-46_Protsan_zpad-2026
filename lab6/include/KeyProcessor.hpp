#pragma once
#include <opencv2/opencv.hpp>

enum class Mode { NORMAL, INVERT, BLUR, CANNY, GLITCH };

class KeyProcessor {
private:
    Mode currentMode = Mode::NORMAL;
public:
    bool processKey(int key);
    Mode getMode() const;
};