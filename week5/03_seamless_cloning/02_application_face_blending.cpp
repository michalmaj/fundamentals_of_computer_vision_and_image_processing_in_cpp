#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <format>
#include <vector>
#include <filesystem>
#include <ranges>

int main() {
    // Set paths to images
    std::string src_path{ "../data/images/obama.jpg" };
    std::string dst_path{ "../data/images/trump.jpg" };
    std::string mask_path{ "../data/images/obama-mask.jpg" };

    // Check if files exist
    try {
        if (!std::filesystem::exists(src_path)) {
            throw std::runtime_error(std::format("Can't load an image from: {}", src_path));
        }
        if (!std::filesystem::exists(dst_path)) {
            throw std::runtime_error(std::format("Can't load an image from: {}", dst_path));
        }
        if (!std::filesystem::exists(mask_path)) {
            throw std::runtime_error(std::format("Can't load an image from: {}", mask_path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Load images from given paths
    cv::Mat src{ cv::imread(src_path) };
    cv::Mat dst{ cv::imread(dst_path) };
    cv::Mat mask{ cv::imread(mask_path, cv::IMREAD_GRAYSCALE) };

    // 1. Simple alpha blending with mask
    // Create alpha with 3 channels
    cv::Mat alpha;
    cv::cvtColor(mask, alpha, cv::COLOR_GRAY2BGR);

    // Convert images into 32-bits float numbers
    alpha.convertTo(alpha, CV_32FC3);
    alpha = alpha / 255.0;
    src.convertTo(src, CV_32FC3);
    dst.convertTo(dst, CV_32FC3);

    // Split channels
    std::vector<cv::Mat> src_channels(3), dst_channels(3), alpha_channels(3), blend_channels(3);
    cv::split(src, src_channels);
    cv::split(dst, dst_channels);
    cv::split(alpha, alpha_channels);

    // Blend channels
    auto view = std::views::zip(src_channels, dst_channels, alpha_channels, blend_channels);

    std::ranges::for_each(view, [](auto&& tuple) {
        auto&& [sr, dt, al, bl] = tuple;
        bl = sr.mul(al) + dt.mul(1 - al);
        });

    cv::Mat blended;
    cv::merge(blend_channels, blended);
    blended.convertTo(blended, CV_8UC3);

    // 2. Perform Seamless Cloning
    // Find center of the mask
    cv::Mat mask_bin;
    cv::threshold(mask, mask_bin, 128, 255, cv::THRESH_BINARY);

    // Calculate moments
    auto m = cv::moments(mask_bin);

    // Set center of mask
    cv::Point center{ static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00) };

    // Clone images
    cv::Mat cloned;

    src.convertTo(src, CV_8UC3);
    dst.convertTo(dst, CV_8UC3);
    mask.convertTo(mask, CV_8UC3);

    cv::seamlessClone(src, dst, mask, center, cloned, cv::NORMAL_CLONE);


    // Show images
    cv::imshow("Obama", src);
    cv::imshow("Trump", dst);
    cv::imshow("Mask", mask);
    cv::imshow("Naive blend", blended);
    cv::imshow("Cloned", cloned);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
