#include <execution>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <random>
#include <ranges>
#include <stdexcept>
#include <vector>

int main() {
    // Path to an image 
    std::string path{ "../data/images/blob_detection.jpg" };

    cv::Mat img{ cv::imread(path, cv::IMREAD_GRAYSCALE) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load image from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Clone the image
    auto blob{ img.clone() };

    // Convert that image into 3-channel image
    cv::cvtColor(blob, blob, cv::COLOR_GRAY2BGR);

    // Set up detector with default parameters
    cv::Ptr<cv::SimpleBlobDetector> detector{ cv::SimpleBlobDetector::create() };
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(img, keypoints);

    // Generate random values in range 0-255
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 255);

    std::ranges::for_each(keypoints, [&blob, &dist, &gen](const cv::KeyPoint& key) {
        // Convert floating-point coordinates to integer
        cv::Point center{ static_cast<int>(key.pt.x), static_cast<int>(key.pt.y) };

        // Mark the center of the blob in red
        cv::circle(blob, center, 5, cv::Scalar(0, 0, 255), -1);

        // Mark every blob in different color
        double diameter{ key.size };
        int radius{ static_cast<int>(diameter / 2) };
        auto color{ cv::Scalar(dist(gen), dist(gen), dist(gen)) };
        cv::circle(blob, center, radius, color, 2);
        });

    // Setting parameters for SimpleBlobDetector
    cv::SimpleBlobDetector::Params params;

    // Set threshold
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by area
    params.filterByArea = true;
    params.minArea = 150;

    // Filter by circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

    detector = cv::SimpleBlobDetector::create(params);
    detector->detect(img, keypoints);

    // Clone the image
    auto blob_parametrized{ img.clone() };

    // Convert that image into 3-channel image
    cv::cvtColor(blob_parametrized, blob_parametrized, cv::COLOR_GRAY2BGR);

    std::ranges::for_each(keypoints, [&blob_parametrized, &dist, &gen](const cv::KeyPoint& key) {
        // Convert floating-point coordinates to integer
        cv::Point center{ static_cast<int>(key.pt.x), static_cast<int>(key.pt.y) };

        // Mark the center of the blob in red
        cv::circle(blob_parametrized, center, 5, cv::Scalar(0, 0, 255), -1);

        // Mark every blob in different color
        double diameter{ key.size };
        int radius{ static_cast<int>(diameter / 2) };
        auto color{ cv::Scalar(dist(gen), dist(gen), dist(gen)) };
        cv::circle(blob_parametrized, center, radius, color, 2);
        });

    cv::imshow("Original", img);
    cv::imshow("Blobs", blob);
    cv::imshow("Blobs Parametrized", blob_parametrized);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
