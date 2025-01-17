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

    cv::Mat dst1, dst2;

    // Apply Gaussian blur, both sigmaX and sigmaY will be calculated automatically
    cv::GaussianBlur(img, dst1, cv::Size(5, 5), 0, 0);

    // Apply Gaussian blur with fixed sigmaX and sigmaY
    cv::GaussianBlur(img, dst2, cv::Size(5, 5), 50, 50);

    cv::imshow("Original", img);
    cv::imshow("Automated sigmas", dst1);
    cv::imshow("Fixed sigmas", dst2);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
