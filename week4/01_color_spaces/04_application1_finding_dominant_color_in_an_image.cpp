#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <print>
#include <stdexcept>
#include <matplot/matplot.h>
#include <vector>

int main() {
    // Path to an image 
    std::string path{ "../data/images/jersey.jpg" };

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

    // Convert image to hsv colorspace
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // Split hsv image into three separate channels (Hue, Value, Saturation)
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);

    // Prepare data for a histogram (only Hue channel)
    std::vector<uint8_t> hue;
    hue.assign(channels.at(0).data, channels.at(0).data + channels.at(0).total());

    // Use matplot++ to show the histogram
    auto his = matplot::hist(hue);
    matplot::show();


    return 0;
}
