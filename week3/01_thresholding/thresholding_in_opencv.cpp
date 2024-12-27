#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <future>

cv::Mat binaryThreshold(const cv::Mat& img, int thresh, int max_value) {
    cv::Mat binary_thresh;
    cv::threshold(img, binary_thresh, thresh, max_value, cv::THRESH_BINARY);
    return binary_thresh;
}

cv::Mat inverseBinaryThreshold(const cv::Mat& img, int thresh, int max_value) {
    cv::Mat binary_inv_thresh;
    cv::threshold(img, binary_inv_thresh, thresh, max_value, cv::THRESH_BINARY_INV);
    return binary_inv_thresh;
}

cv::Mat truncateThreshold(const cv::Mat& img, int thresh, int max_value) {
    cv::Mat truncate_thresh;
    cv::threshold(img, truncate_thresh, thresh, max_value, cv::THRESH_TRUNC);
    return truncate_thresh;
}

cv::Mat toZeroThreshold(const cv::Mat& img, int thresh, int max_value) {
    cv::Mat to_zero_thresh;
    cv::threshold(img, to_zero_thresh, thresh, max_value, cv::THRESH_TOZERO);
    return to_zero_thresh;
}

cv::Mat inverseToZeroThreshold(const cv::Mat& img, int thresh, int max_value) {
    cv::Mat to_zero_inv_thresh;
    cv::threshold(img, to_zero_inv_thresh, thresh, max_value, cv::THRESH_TOZERO_INV);
    return to_zero_inv_thresh;
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/threshold.png" };

    // Load the image
    cv::Mat img{ cv::imread(path) };

    // Check if the image loaded correctly
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Set threshold and max value
    int thresh{ 100 };
    int max_value{ 150 };

    auto binary_future = std::async(std::launch::async, binaryThreshold, std::cref(img), thresh, max_value);
    auto binary_inv_future = std::async(std::launch::async, inverseBinaryThreshold, std::cref(img), thresh, max_value);
    auto truncate_future = std::async(std::launch::async, truncateThreshold, std::cref(img), thresh, max_value);
    auto to_zero_future = std::async(std::launch::async, toZeroThreshold, std::cref(img), thresh, max_value);
    auto to_zero_inv_future = std::async(std::launch::async, inverseToZeroThreshold, std::cref(img), thresh, max_value);

    cv::Mat binary_thresh{ binary_future.get() };
    cv::Mat binary_inv_thresh{ binary_inv_future.get() };
    cv::Mat truncate_thresh{ truncate_future.get() };
    cv::Mat to_zero_thresh{ to_zero_future.get() };
    cv::Mat to_zero_inv_thresh{ to_zero_inv_future.get() };

    cv::imshow("Original", img);
    cv::imshow("Binary Threshold", binary_thresh);
    cv::imshow("Binary Inverse Threshold", binary_inv_thresh);
    cv::imshow("Truncate Threshold", truncate_thresh);
    cv::imshow("To Zero Threshold", to_zero_thresh);
    cv::imshow("To Zero Inverse Threshold", to_zero_inv_thresh);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
