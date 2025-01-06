#include <execution>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <print>
#include <stdexcept>
#include <vector>


struct CoinDetection {
    cv::Mat src, gray, processed;
    std::vector<cv::Mat> split;

    // Window name
    const std::string image_window{ "Processed Image" };
    const std::string thresh_window{ "Threshold Controls" };

    // Type of operations
    /*
     * 1. Thresholding
     * 2. Morphological Operations
     * 3. Blob Detection
     * 4. Contour Detection
     * 5. Connected Component Analysis
     */

     // Threshold operations
    int threshold_type{ 0 };
    int threshold_steps{ 4 };
    int threshold_min_value{ 0 };
    int threshold_max_value{ 255 };
    int steps{ 255 };
    std::string number_threshold_types{ "Threshold Controls" };
    std::string min_thresh{ "Threshold Min Value" };
    std::string max_thresh{ "Threshold Max Value" };
};

void thresholdImage(CoinDetection& cd) {
    cv::threshold(cd.gray, cd.processed, cd.threshold_min_value, cd.threshold_max_value, cd.threshold_type);
}

void ProcessThreshold(int, void* data) {
    auto* cd = static_cast<CoinDetection*>(data);
    thresholdImage(*cd);
    cv::imshow(cd->image_window, cd->processed);
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/CoinsA.png" };

    // Load an image
    cv::Mat img{ cv::imread(path) };

    // Check if image is loaded
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load image from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    CoinDetection cd;
    cd.src = img.clone();
    cv::split(cd.src, cd.split);
    cv::cvtColor(cd.src, cd.gray, cv::COLOR_BGR2GRAY);
    cd.processed = cd.gray.clone();
    cv::namedWindow(cd.image_window, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(cd.thresh_window, cv::WINDOW_AUTOSIZE);

    // Create trackbars for thresholding
    cv::createTrackbar(cd.number_threshold_types, cd.thresh_window, &cd.threshold_type, cd.threshold_steps, ProcessThreshold, &cd);
    cv::createTrackbar(cd.min_thresh, cd.thresh_window, &cd.threshold_min_value, cd.steps, ProcessThreshold, &cd);
    cv::createTrackbar(cd.max_thresh, cd.thresh_window, &cd.threshold_max_value, cd.steps, ProcessThreshold, &cd);

    // Create trackbars for Morphological Operations

    // Initial windo
    cv::imshow(cd.image_window, cd.src);

    while (true) {
        auto key{ cv::waitKey(10) };

        if (key == 'q') {
            break;
        }

        if (key == 'c') {
            cd.processed = cd.gray.clone();
            cv::imshow(cd.image_window, cd.gray);
        }
        else if (key == 'b') {
            cd.processed = cd.split.at(0).clone();
            cv::imshow(cd.image_window, cd.split.at(0));
        }
        else if (key == 'g') {
            cd.processed = cd.split.at(1).clone();
            cv::imshow(cd.image_window, cd.split.at(1));
        }
        else if (key == 'r') {
            cd.processed = cd.split.at(2).clone();
            cv::imshow(cd.image_window, cd.split.at(2));
        }
    }

    cv::destroyWindow(cd.image_window);

    return 0;
}
