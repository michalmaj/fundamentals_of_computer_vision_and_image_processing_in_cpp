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

    std::string face_mask{ "Face Mask" };
    cv::namedWindow(face_mask, cv::WINDOW_NORMAL);

    std::string color_mask{ "Color Mask" };
    cv::namedWindow(color_mask, cv::WINDOW_NORMAL);

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

    // Create a binary mask, when white pixels will be at face area
    cv::Mat mask1 = cv::Mat::zeros(img.size(), img.type());
    mask1(cv::Range(50, 200), cv::Range(170, 320)).setTo(255);

    // Create color mask
    cv::Mat mask2;
    // Set colors in different ranges: Blue [0-150], Green [0-150], Red [100-255]
    cv::inRange(img, cv::Scalar(0, 0, 150), cv::Scalar(100, 100, 255), mask2);

    cv::imshow(original, img);
    cv::imshow(face_mask, mask1);
    cv::imshow(color_mask, mask2);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
