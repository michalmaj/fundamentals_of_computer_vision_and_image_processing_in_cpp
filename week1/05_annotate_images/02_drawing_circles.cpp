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

    cv::Mat img_circle = img.clone();
    cv::Mat img_circle_filled = img.clone();

    auto center = cv::Point(250, 125);
    auto radius = 100;
    auto color = cv::Scalar(255, 0, 0); // Blue
    auto thickness = 5;
    auto line_type = cv::LINE_AA;
    cv::circle(img_circle, center, radius, color, thickness, line_type);
    cv::circle(img_circle_filled, center, radius, color, -thickness, line_type); // negative thickness to fill circle

    cv::imshow("Circle", img_circle);
    cv::imshow("Filled Circle", img_circle_filled);
    cv::imshow("Original image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
