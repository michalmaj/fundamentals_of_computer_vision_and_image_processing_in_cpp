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

    cv::Mat ycrcb;
    cv::Mat lab;

    cv::cvtColor(img, ycrcb, cv::COLOR_BGR2YCrCb);
    cv::cvtColor(img, lab, cv::COLOR_BGR2Lab);

    std::vector<cv::Mat> channels_ycrcb;
    std::vector<cv::Mat> channels_lab;
    std::array<std::string, 3> ycrcb_names{ "Luma", "R - Y", "B - Y" };
    std::array<std::string, 3> lab_names{ "Lightness", "Green to Magenta", "Blue to Yellow" };

    cv::split(ycrcb, channels_ycrcb);
    cv::split(lab, channels_lab);

    auto ycrcb_view = std::views::zip(ycrcb_names, channels_ycrcb);
    auto lab_view = std::views::zip(lab_names, channels_lab);

    cv::imshow("Original", img);
    cv::imshow("YCrCb", ycrcb);
    cv::imshow("Lab", lab);

    for (auto&& [name, channel] : ycrcb_view) {
        cv::imshow(name, channel);
    }

    for (auto&& [name, channel] : lab_view) {
        cv::imshow(name, channel);
    }

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
