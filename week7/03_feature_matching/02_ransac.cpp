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
    std::filesystem::path path1{ "../data/images/book.jpeg" };
    if (!std::filesystem::exists(path1)) {
        std::cerr << std::format("Can't find file at given location: {}", path1.string());
        return EXIT_FAILURE;
    }

    std::filesystem::path path2{ "../data/images/book_scene.jpeg" };
    if (!std::filesystem::exists(path2)) {
        std::cerr << std::format("Can't find file at given location: {}", path2.string());
        return EXIT_FAILURE;
    }

    // Load images
    cv::Mat img1{ cv::imread(path1.string()) };
    cv::Mat img2{ cv::imread(path2.string()) };

    // Convert images to grayscale
    cv::Mat gray1, gray2;
    cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    // Create ORB detector
    cv::Ptr<cv::ORB> orb{ cv::ORB::create() };

    // Find keypoints and descriptors
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat desc1, desc2;
    orb->detectAndCompute(gray1, cv::Mat{}, kp1, desc1);
    orb->detectAndCompute(gray2, cv::Mat{}, kp2, desc2);

    // Brute-force matcher
    cv::BFMatcher bf{ cv::NORM_HAMMING, true };

    // Match descriptors
    std::vector<cv::DMatch> matches;
    bf.match(desc1, desc2, matches);

    // Sort matches by distance
    std::ranges::sort(matches, std::less{});

    // Take only 10 best matches
    matches = matches | std::views::take(10) | std::ranges::to<std::vector>();

    // Create points for homography
    std::vector<cv::Point> src_pts, dst_pts;
    for (const auto& match : matches) {
        src_pts.emplace_back(kp1[match.queryIdx].pt);
        dst_pts.emplace_back(kp2[match.queryIdx].pt);
    }

    // Calculate homography with RANSAC algorithm to create mask
    cv::Mat mask;
    auto M = cv::findHomography(src_pts, dst_pts, cv::RANSAC, 1.5, mask);

    cv::Mat output;
    cv::drawMatches(img1, kp1, img2, kp2, matches, output,
        cv::Scalar::all(-1),
        cv::Scalar::all(-1), mask);

    std::string output_window_name{ "Output" };
    cv::namedWindow(output_window_name, cv::WINDOW_NORMAL);
    cv::imshow(output_window_name, output);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
