#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <print>
#include <stdexcept>
#include <vector>
#include <ranges>


int main() {
    // Path to an image 
    std::string path{ "../data/images/capsicum.jpg" };

    // Load an image
    cv::Mat img{ cv::imread(path) };

    // Check if image is loaded
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load image from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    std::array<std::string, 3> names{ "Hue", "Saturation", "Value" };

    auto view = std::views::zip(names, channels);

    cv::imshow("Original", img);
    cv::imshow("HSV", hsv);
    for (auto&& [name, channel] : view) {
        cv::imshow(name, channel);
    }

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
