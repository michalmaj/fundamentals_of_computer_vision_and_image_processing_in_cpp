#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <filesystem>
#include <print>
#include <ranges>


int main() {
    std::filesystem::path path{ "../data/images/book.jpeg" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't find file at given location: {}", path.string());
        return EXIT_FAILURE;
    }

    // Load an image from given path
    cv::Mat img{ cv::imread(path.string()) };
    std::string original_window_name{ "Original" };
    if (img.empty()) {
        std::cerr << std::format("Can't load an image from given path: {}", path.string());
        return EXIT_FAILURE;
    }

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    std::string gray_window_name{ "Grayscale" };

    // Initialize ORB detector
    cv::Ptr<cv::ORB> orb = cv::ORB::create();

    // Create space for keypoints
    std::vector<cv::KeyPoint> kp;
    kp.reserve(500); // Default value for ORB::create method

    // Find keypoints
    orb->detect(gray, kp, cv::Mat());

    // Compute the descriptors by given keypoints
    cv::Mat descriptors;
    orb->compute(gray, kp, descriptors);

    // Draw keypoints
    cv::Mat computed_500_kp;
    std::string computed_500_window_name{ "500 keypoints computed" };
    cv::drawKeypoints(img, kp, computed_500_kp, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // Create only 10 features
    orb = cv::ORB::create(10);
    kp.clear();
    kp.reserve(10);
    orb->detectAndCompute(gray, cv::Mat(), kp, descriptors);

    cv::Mat computed_10_kp;
    std::string computed_10_window_name{ "10 keypoints computed" };
    cv::drawKeypoints(img, kp, computed_10_kp, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


    // Create named windows for display purpose
    cv::namedWindow(original_window_name, cv::WINDOW_NORMAL);
    cv::namedWindow(gray_window_name, cv::WINDOW_NORMAL);
    cv::namedWindow(computed_500_window_name, cv::WINDOW_NORMAL);
    cv::namedWindow(computed_10_window_name, cv::WINDOW_NORMAL);

    // Show images
    cv::imshow(original_window_name, img);
    cv::imshow(gray_window_name, gray);
    cv::imshow(computed_500_window_name, computed_500_kp);
    cv::imshow(computed_10_window_name, computed_10_kp);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
