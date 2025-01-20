#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/sample.jpg" };

    // Load the image in grayscale, 2nd derivative can be only calculated on that type of image
    cv::Mat img{ cv::imread(path, cv::IMREAD_GRAYSCALE) };

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

    cv::Mat gaussian, laplacian;

    // Apply Gaussian blur before apply Laplacian
    cv::GaussianBlur(img, gaussian, cv::Size(3, 3), 0, 0);

    // Apply Laplacian
    cv::Laplacian(gaussian, laplacian, CV_32F, 3, 1, 0);

    // Normalize image to range 0-1
    cv::normalize(laplacian, laplacian, 0, 1, cv::NORM_MINMAX);

    cv::imshow("Original", img);
    cv::imshow("Gaussian", gaussian);
    cv::imshow("Laplacian", laplacian);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
