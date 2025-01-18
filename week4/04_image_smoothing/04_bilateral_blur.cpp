#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/gaussian-noise.png" };

    // Load the image
    cv::Mat img{ cv::imread(path) };

    // Check if the image is loaded correctly
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    cv::Mat filtered;

    // Apply bilateral filter with given diameter (non-negative)
    int diameter{ 15 };
    int sigma_color{ 80 };
    int sigma_space{ 80 };
    cv::bilateralFilter(img, filtered, diameter, sigma_color, sigma_space);


    cv::imshow("Original", img);
    cv::imshow("With diameter", filtered);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
