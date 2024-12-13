#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <print>
#include <unordered_map>

// Map OpenCV depth type to a human-readable string
std::string depth2String(int depth) {
    const std::unordered_map<int, std::string> depths{
        {CV_8U, "8U"},
        {CV_8S, "8S"},
        {CV_16U, "16U"},
        {CV_16S, "16S"},
        {CV_32S, "32S"},
        {CV_32F, "32F"},
        {CV_64F, "64F"}
    };

    return (depths.contains(depth)) ? depths.at(depth) : "Unknown";
}

int main() {
    // Create a name for OpenCV window
    std::string windowName{ "Image" };
    // Use function namedWindow to allow resizing window by mouse (WINDOW_NORMAL flag is necessary).
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    
    // Load an image from given path, flag IMREAD_GARYSCALE loads image as 1 channel grayscale image
    cv::Mat img{ cv::imread("../data/images/number_zero.jpg", cv::IMREAD_GRAYSCALE) };

    // Check if loaded image is present, if not 
    if (img.empty()) {
        std::cerr << "Cannot open the file\n";
        return EXIT_FAILURE;
    }

    // Use C++17 feature (structure binding) to get image width and height
    auto [width, height] = img.size();
    // Use C++23 feature to print with new line
    std::println("Image width: {}, height: {}", width, height);

    // Check depth (how many bits there are) and number of channels (e.g. grayscale has 1 channel)
    auto depth{ depth2String(img.depth()) };
    auto channels{ img.channels()};
    std::println("Image depth: {}, channels: {}", depth, channels);

    // Show image in given window (by name)
    cv::imshow(windowName, img);
    // Wait for n millicesonds - 0 indicated infinite time
    cv::waitKey(0);
    // Clean up by destroying the window
    cv::destroyWindow(windowName);

}