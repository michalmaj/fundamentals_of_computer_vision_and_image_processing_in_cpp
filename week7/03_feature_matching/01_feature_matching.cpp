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

    // Brute-force macher
    // Create BFMacher
    cv::BFMatcher bf(cv::NORM_HAMMING, true);

    // Match descriptors
    std::vector<cv::DMatch> matches;
    bf.match(desc1, desc2, matches, cv::Mat{});

    // Sort matches by their distance
    std::ranges::sort(matches, std::less{});

    // Take only 10 best matches
    auto match_bf_n = matches | std::views::take(10) | std::ranges::to<std::vector>();

    cv::Mat output_bf;

    std::vector<char> match_mask(match_bf_n.size(), 1);

    cv::drawMatches(
        img1,
        kp1,
        img2,
        kp2,
        match_bf_n,
        output_bf,
        cv::Scalar::all(-1),
        cv::Scalar::all(-1),
        match_mask,
        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // FLANN Matcher
    cv::FlannBasedMatcher flann(
        new cv::flann::LshIndexParams(6, 12, 1),
        new cv::flann::SearchParams(50)
    );

    std::vector<std::vector<cv::DMatch>> matches_vec;
    flann.knnMatch(desc1, desc2, matches_vec, 2);

    auto match_flann_n = matches_vec | std::views::take(10) | std::ranges::to<std::vector>();

    cv::Mat output_flann;
    cv::drawMatches(img1, kp1, img2, kp2, match_flann_n, output_flann);

    // Windows properties for display purpose
    std::string bf_window_name{ "Brute-force" };
    std::string flann_window_name{ "Flann" };
    cv::namedWindow(bf_window_name, cv::WINDOW_NORMAL);
    cv::namedWindow(flann_window_name, cv::WINDOW_NORMAL);

    cv::imshow(bf_window_name, output_bf);
    cv::imshow(flann_window_name, output_flann);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
