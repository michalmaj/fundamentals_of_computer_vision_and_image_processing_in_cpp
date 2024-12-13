#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <print>

int main() {
    // Create a name for the original OpenCV window
    std::string windowName{ "Image" };
    // Allow resizing the window by mouse (WINDOW_NORMAL flag is necessary)
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);


    // Load an image from the given path. IMREAD_COLOR loads the image as a three-channels BGR image.
    cv::Mat img{ cv::imread("../data/images/musk.jpg", cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error("Error: Cannot open the file ../data/images/musk.jpg\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    auto [width, height] = img.size();
    std::println("Image width and height: [{},{}], number of channels: {}", width, height, img.channels());

    cv::imshow(windowName, img);
    cv::waitKey(0);
    cv::destroyWindow(windowName);
    
}
