#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>


int main() {
    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string diagonal{ "Color Diagonally" };
    cv::namedWindow(diagonal, cv::WINDOW_NORMAL);

    std::string vertical{ "Color Vertically" };
    cv::namedWindow(vertical, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_COLOR loads the image as a three-channels BGR image.
    // It is possible even if given image is in the grayscale, simply all channels will be contain the same grayscale image
    cv::Mat img{ cv::imread("../data/images/number_zero.jpg", cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error("Error: Cannot open the file ../data/images/number_zero.jpg\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    std::cout << "Values of pixels ot given location:\n" << img.at<cv::Vec3b>(0, 0) << "\n\n";

    cv::Mat addColorsDiagonally = img.clone();

    // Set new color at given pixels location
    addColorsDiagonally.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 255, 255); // Yellow
    addColorsDiagonally.at<cv::Vec3b>(1, 1) = cv::Vec3b(255, 255, 0); // Cyan
    addColorsDiagonally.at<cv::Vec3b>(2, 2) = cv::Vec3b(255, 0, 255); // Magenta

    cv::Mat addColorsVertically = img.clone();
    auto stripe1 = cv::Range(0, 3);
    auto stripe2 = cv::Range(3, 6);
    auto stripe3 = cv::Range(6, 9);
    auto width = cv::Range(0, 3);
    addColorsVertically(stripe1, width).setTo(cv::Scalar(255, 0, 0));
    addColorsVertically(stripe2, width).setTo(cv::Scalar(0, 255, 0));
    addColorsVertically(stripe3, width).setTo(cv::Scalar(0, 0, 255));

    cv::imshow(original, img);
    cv::imshow(diagonal, addColorsDiagonally);
    cv::imshow(vertical, addColorsVertically);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
