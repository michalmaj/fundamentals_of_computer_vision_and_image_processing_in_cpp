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

    // Use the kernel of 3x3
    cv::blur(img, dst1, cv::Size(3, 3));

    // Use the kernel of 7x7
    cv::blur(img, dst2, cv::Size(7, 7));

    cv::imshow("Original", img);
    cv::imshow("Kernel 3x3", dst1);
    cv::imshow("Kernel 7x7", dst2);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
