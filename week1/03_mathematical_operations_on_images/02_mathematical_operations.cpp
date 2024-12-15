#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Path to image
    std::string path{ "../data/images/boy.jpg" };

    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_COLOR loads the image as a BGR image.
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Error: Cannot open the file {}\n", path));
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Image normalization to floating point (32-bits) in range 0-1
    cv::Mat normalized;
    cv::normalize(img, normalized, 0, 1, cv::NORM_MINMAX, CV_32FC3);

    // Change contrast of an image
    double contrast_scale = 1 + 0.5;
    cv::Mat high_contrast = normalized * contrast_scale;

    // Change brightness of an image
    std::vector<cv::Mat> channels;
    cv::split(normalized, channels);

    double brightness_scale = 100 / 255.0;
    for (auto& e : channels) {
        cv::add(e,brightness_scale , e);
    }

    cv::Mat brightness;
    cv::merge(channels, brightness);

    cv::imshow(original, img);
    cv::imshow("Normalized", normalized);
    cv::imshow("Contrast", high_contrast);
    cv::imshow("Change brightness", brightness);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
