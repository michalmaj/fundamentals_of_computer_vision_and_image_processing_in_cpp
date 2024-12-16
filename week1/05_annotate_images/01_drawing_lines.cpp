#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>


int main() {
    // Path to an image 
    std::string path{ "../data/images/boy.jpg" };
    // Load the image
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    cv::Mat img_line = img.clone();

    auto start = cv::Point(200, 80);
    auto end = cv::Point(280, 80);
    auto color = cv::Scalar(0, 255, 0); // Green line
    auto thickness = 3; // Thickness of 3 pixels
    auto line_type = cv::LINE_AA;

    cv::line(img_line, start, end, color, thickness, line_type);

    cv::imshow("Original Image", img);
    cv::imshow("Image with line", img_line);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
