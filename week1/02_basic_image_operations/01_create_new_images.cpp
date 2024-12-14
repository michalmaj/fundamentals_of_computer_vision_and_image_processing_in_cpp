#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string blackWindow{ "Black Canvas" };
    cv::namedWindow(blackWindow, cv::WINDOW_NORMAL);

    std::string whiteWindow{ "White Canvas" };
    cv::namedWindow(whiteWindow, cv::WINDOW_NORMAL);

    std::string emptyCanvas{ "Empty Canvas" };
    cv::namedWindow(emptyCanvas, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_UNCHANGED loads the image as a four-channels aBGR image (if possible).
    std::string path{ "../data/images/boy.jpg" };
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

    // Create an empty matrix
    int height = 100;
    int width = 200;
    auto type = CV_8UC3;
    auto scalar = cv::Scalar(0, 0, 0); // black pixels
    cv::Mat blackCanvas = cv::Mat(height, width, type, scalar);

    // Another method, directly use ::ones to fill Mat object with white color
    type = CV_32F;
    cv::Mat whiteCanvas = cv::Mat::ones(height, width, type);

    // Canvas by given image
    auto canvas = cv::Mat::zeros(img.size(), img.type());

    cv::imshow(original, img);
    cv::imshow(blackWindow, blackCanvas);
    cv::imshow(whiteWindow, whiteCanvas);
    cv::imshow(emptyCanvas, canvas);
    cv::waitKey(0);
    cv::destroyAllWindows();

}
