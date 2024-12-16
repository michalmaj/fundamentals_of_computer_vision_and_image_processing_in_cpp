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

    cv::Mat img_ellipse = img.clone();
    cv::Mat img_ellipse_filled = img.clone();

    // Set variables for ellipse
    auto center = cv::Point(250, 125);
    auto major_minor_axes = cv::Point(100, 50);
    auto angle = 0.0; // Alignment to x-axis
    auto start_angle = 0; // How many (in degrees) draw an ellipse (start)
    auto stop_angle = 360; // How many (in degrees) draw an ellipse (end)
    auto color = cv::Scalar(255, 0, 0); // Blue
    auto thickness = 5;
    auto line_type = cv::LINE_AA;

    // Draw ellipses
    cv::ellipse(img_ellipse, center, major_minor_axes, angle, start_angle, stop_angle, color, thickness, line_type);
    angle = 90.0; // Alignment to y-axis
    color = cv::Scalar(0, 255, 0); // Green
    cv::ellipse(img_ellipse, center, major_minor_axes, angle, start_angle, stop_angle, color, thickness, line_type);

    // Draw green ellipse from 180 to 360 degrees
    angle = 0;
    start_angle = 180;
    cv::ellipse(img_ellipse_filled, center, major_minor_axes, angle, start_angle, stop_angle, color, -thickness, line_type); // negative thickness to fill circle

    // Draw blue ellipse from 0 to 180 degrees
    start_angle = 0;
    stop_angle = 180;
    color = cv::Scalar(255, 0, 0); // Blue
    cv::ellipse(img_ellipse_filled, center, major_minor_axes, angle, start_angle, stop_angle, color, -thickness, line_type); // negative thickness to fill circle

    cv::imshow("Ellipse", img_ellipse);
    cv::imshow("Filled Ellipse", img_ellipse_filled);
    cv::imshow("Original image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
