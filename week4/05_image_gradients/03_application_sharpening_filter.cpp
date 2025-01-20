#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/sample.jpg" };

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

    // Define a kernel for a convolution
    cv::Matx<int, 3, 3> sharpen = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };

    // Create an empty Mat for sharpened image
    cv::Mat sharpened_image;

    // Apply filter 2D (-1 indicates same depth in output image)
    cv::filter2D(img, sharpened_image, -1, sharpen);

    cv::imshow("Original", img);
    cv::imshow("Sharpened", sharpened_image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
