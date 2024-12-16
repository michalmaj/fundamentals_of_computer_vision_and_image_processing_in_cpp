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

    cv::Mat img_rectangle = img.clone();
    cv::Mat img_rectangle_filled = img.clone();

    auto start_point = cv::Point(170, 50);
    auto end_point = cv::Point(300, 200);
    auto color = cv::Scalar(255, 0, 255); // Magenta
    auto thickness = 5;
    auto line_type = cv::LINE_AA;

    cv::rectangle(img_rectangle, start_point, end_point, color, thickness, line_type);
    cv::rectangle(img_rectangle_filled, start_point, end_point, color, -thickness, line_type);

    cv::imshow("Rectangle", img_rectangle);
    cv::imshow("Filled Rectangle", img_rectangle_filled);
    cv::imshow("Original image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
