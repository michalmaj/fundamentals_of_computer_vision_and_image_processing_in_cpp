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
    std::string path{ "../data/images/lanes.jpg" };

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

    // Use Canny detector to detect edges
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 200);

    // Detect points in the image
    std::vector<cv::Vec4i> lines;

    // Find line only if 100 pixels voted on that
    cv::HoughLinesP(edges, lines, 1, std::numbers::pi / 180, 100, 10, 250);

    // Draw lines on the image
    std::ranges::for_each(lines, [&img](const auto& line) {
        cv::line(img, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
        });

    cv::imshow("Lines", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
