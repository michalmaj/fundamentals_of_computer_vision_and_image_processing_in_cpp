#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <format>
#include <vector>
#include <numbers>
#include <ranges>

int main() {
    // Path to an image 
    std::string path{ "../data/images/circles.jpg" };

    cv::Mat img{ cv::imread(path) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Blur the image
    cv::Mat blurred;
    cv::medianBlur(gray, blurred, 5);

    // Find circles
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, 50, 450, 10, 30, 40);

    std::ranges::for_each(circles, [&img](const auto& circle) {
        cv::circle(img, 
            cv::Point(static_cast<int>(circle[0]), static_cast<int>(circle[1])), 
            static_cast<int>(circle[2]), 
            cv::Scalar(255, 0, 255), 
            2, 
            cv::LINE_AA);
        });

    cv::imshow("Circles", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
