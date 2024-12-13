#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Create a name for the original OpenCV window
    std::string windowName{ "Image" };
    // Allow resizing the window by mouse (WINDOW_NORMAL flag is necessary)
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);


    // Load an image from the given path. IMREAD_COLOR loads the image as a three-channels BGR image.
    cv::Mat img{ cv::imread("../data/images/boy.jpg", cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error("Error: Cannot open the file ../data/images/boy.jpg\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Save image to disk: path, cv::Mat, params (https://docs.opencv.org/4.1.0/d4/da8/group__imgcodecs.html#ga292d81be8d76901bff7988d18d2b42ac)
    cv::imwrite("../data/images/boy_clone.jpg", img);

    cv::imshow(windowName, img);
    cv::waitKey(0);
    cv::destroyWindow(windowName);
}
