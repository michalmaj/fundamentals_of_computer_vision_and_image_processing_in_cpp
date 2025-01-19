#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/truth.png" };

    // Load the image in grayscale, gradients can be only calculated on that type of image
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

    cv::Mat sobelx, sobely, sobelxy;

    // Apply sobel filter with only x gradient
    cv::Sobel(img, sobelx, CV_32F, 1, 0);

    // Apply sobel filter with only y gradient
    cv::Sobel(img, sobely, CV_32F, 0, 1);

    // Apply sobel filter in with both x and y gradients
    cv::Sobel(img, sobelxy, CV_32F, 1, 1);

    // Normalize all filtered images to range [0-1]
    cv::normalize(sobelx, sobelx, 0, 1, cv::NORM_MINMAX);
    cv::normalize(sobely, sobely, 0, 1, cv::NORM_MINMAX);
    cv::normalize(sobelxy, sobelxy, 0, 1, cv::NORM_MINMAX);


    cv::imshow("Original", img);
    cv::imshow("X Gradients", sobelx);
    cv::imshow("Y Gradients", sobely);
    cv::imshow("XY Gradients", sobelxy);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
