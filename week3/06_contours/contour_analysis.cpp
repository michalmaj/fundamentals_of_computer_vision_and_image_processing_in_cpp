#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <print>
#include <random>
#include <ranges>

/// <summary>
/// Wrapper around OpenCV function named cv::findContours. In this case it returns std::pair that contains:
///     - contours in std::vector<std::vector<cv::Point>>
///     - hierarchy in std::vector<cv::Vec4i>
/// </summary>
/// <param name="img">Input image, must be 8-bit single-channel</param>
/// <param name="mode">Contour retrieval mode, ( RETR_EXTERNAL, RETR_LIST, RETR_CCOMP, RETR_TREE )</param>
/// <param name="method">Contour approximation method. ( CHAIN_APPROX_NONE, CHAIN_APPROX_SIMPLE, CHAIN_APPROX_TC89_L1 etc)</param>
/// <param name="offset">Optional offset by which every contour point is shifted. This is useful if the contours are extracted from the image ROI and then they should be analyzed in the whole image context.</param>
/// <returns>std::tuple that contains contours and hierarchy</returns>
[[nodiscard]] auto findContours(const cv::Mat& img, int mode, int method, cv::Point offset = cv::Point()) {
    if (img.empty()) {
        throw std::runtime_error("Empty image\n");
    }

    if (img.channels() != 1 or img.depth() != CV_8U) {
        throw std::runtime_error("Wrong type of image. Must be 8-bit single channel");
    }

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(img, contours, hierarchy, mode, method, offset);

    return std::pair{ contours, hierarchy };
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/Contour.png" };

    cv::Mat img{ cv::imread(path) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load image from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Find all contour in the image without knowledge if this contour is inside another contour
    auto [contours, hierarchy] = findContours(gray, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    std::println("Found contours by RETR_LIST: {}", contours.size());
    cv::Mat list{ img.clone() };
    cv::drawContours(list, contours, -1, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);

    // Find only external contours, if one contour inside another just skip it
    std::tie(contours, hierarchy) = findContours(gray, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::println("Found contours by RETR_EXTERNAL: {}", contours.size());
    cv::Mat external{ img.clone() };
    cv::drawContours(external, contours, -1, cv::Scalar(255, 0, 0), 3, cv::LINE_AA);

    // Find all as a connected components, this will produce 2 levels of contours
    std::tie(contours, hierarchy) = findContours(gray, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    std::println("Found contours by RETR_CCOPM: {}", contours.size());
    cv::Mat ccomp{ img.clone() };

    // Draw external in yellow and internal as light blue
    for (int i{ 0 }; i < contours.size(); ++i) {
        if (hierarchy[i][3] == -1) { // External contour (no parents)
            cv::drawContours(ccomp, contours, i, cv::Scalar(0, 255, 255), 3, cv::LINE_AA);
        }
        else {
            cv::drawContours(ccomp, contours, i, cv::Scalar(255, 255, 0), 3, cv::LINE_AA);
        }
    }

    // Find all contours as tree
    std::tie(contours, hierarchy) = findContours(gray, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    std::println("Found contours by RETR_TREE: {}", contours.size());
    cv::Mat tree{ img.clone() };

    // Create random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 255);

    // Draw all contours in different colors
    for (int i{ 0 }; i < contours.size(); ++i) {
        // Make random color for every contour
        cv::Scalar color{ cv::Scalar(dist(gen), dist(gen), dist(gen)) };
        cv::drawContours(tree, contours, i, color, 3, cv::LINE_AA);
    }

    // Find center of mass (centroid) and add number of contour
    std::tie(contours, hierarchy) = findContours(gray, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    std::println("Found contours by RETR_LIST: {}", contours.size());
    cv::Mat centroid{ img.clone() };

    // Draw all contours in different color and add centroids and number of contour
    for (int i{ 0 }; i < contours.size(); ++i) {
        cv::Moments m{ cv::moments(contours[i]) };
        int x{ static_cast<int>(m.m10 / m.m00) };
        int y{ static_cast<int>(m.m01 / m.m00) };

        // Make random color for every contour
        cv::Scalar color{ cv::Scalar(dist(gen), dist(gen), dist(gen)) };

        // Draw center of mass
        cv::circle(centroid, cv::Point(x, y), 10, color, -1);

        // Add number to contour
        cv::putText(centroid,
            std::to_string(i + 1),
            cv::Point(x + 40, y - 10),
            cv::FONT_HERSHEY_TRIPLEX,
            1,
            color,
            2);
    }

    // Calculate area and perimeter of every contour
    std::ranges::for_each(contours, [i=1]  (const auto& contour) mutable {
        auto area{ cv::contourArea(contour) };
        auto perimeter{ cv::arcLength(contour, true) };
        std::println("Contour #{} has area: {} and perimeter: {:.6}", i++, area, perimeter);
    });

    cv::imshow("Original", img);
    cv::imshow("Grayscale", gray);
    cv::imshow("Retr List", list);
    cv::imshow("Retr External", external);
    cv::imshow("Retr Ccomp", ccomp);
    cv::imshow("Retr Tree", tree);
    cv::imshow("Centroids", centroid);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
