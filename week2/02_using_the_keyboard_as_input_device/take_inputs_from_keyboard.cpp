#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <print>

int main() {
    cv::VideoCapture cap(0);
    try {
        if (!cap.isOpened()) {
            throw std::runtime_error("Can't open video stream!\n");
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }

    auto width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    auto height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    auto fps = cap.get(cv::CAP_PROP_FPS);

    std::println("Frame width: {}, height: {}, fps: {}", width, height, fps);

    cv::Mat frame;
    std::string winname{ "Frame" };
    cv::namedWindow(winname, cv::WINDOW_NORMAL);
    char key_pressed{};
    while (true) {
        cap.read(frame);

        char key = cv::waitKey(1);

        if (key == 'q') {
            break;
        }

        if (key != -1) {
            key_pressed = key;
        }

        if (tolower(key_pressed) == 'e') {
            std::string text{ std::format("{} is pressed", key_pressed) };
            cv::putText(frame, text, cv::Point(10, 20), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(255, 0, 255));
        }
        else if (key_pressed == 'z') {
            key_pressed = -1;
        }

        cv::imshow(winname, frame);
    }

    cap.release();
    cv::destroyWindow(winname);
}
