#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <print>
#include <vector>
#include <unordered_map>

int main() {
    // Create a name for the original OpenCV window
    std::string windowName{ "Image" };
    // Allow resizing the window by mouse (WINDOW_NORMAL flag is necessary)
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);


    // Load an image from the given path. IMREAD_COLOR loads the image as a three-channels BGR image.
    cv::Mat img{ cv::imread("../data/images/musk.jpg", cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error("Error: Cannot open the file ../data/images/musk.jpg\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Split channels int vector of cv::Mats
    std::vector<cv::Mat> bgr;
    cv::split(img, bgr);

    try {
        if (bgr.size() != 3) {
            throw std::runtime_error("Unexpected number of channels!\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Copy every channel into map of colors
    std::unordered_map<std::string, cv::Mat> channels{
        {"Blue", bgr.at(0)},
        {"Green", bgr.at(1)},
        {"Red", bgr.at(2)}
    };

    // Show images from map
    for (const auto& [color, channel] : channels) {
        cv::imshow(color, channel);
    }

    // Merge channels back into single image
    cv::Mat merged;
    cv::merge(bgr, merged);

    cv::imshow(windowName, img);
    cv::imshow("Merged", merged);
    cv::waitKey(0);
    cv::destroyAllWindows();
    
}
