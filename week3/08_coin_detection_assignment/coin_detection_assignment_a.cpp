#include <execution>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <ranges>
#include <stdexcept>
#include <vector>


struct UserData {
    cv::Mat src, gray, processed;

    // Window name
    const std::string named_window{ "Processed Image" };
    
    // Type of operations
    /*
     * 1. Thresholding
     * 2. Morphological Operations
     * 3. Blob Detection
     * 4. Contour Detection
     * 5. Connected Component Analysis
     */
    int operation_type{ 0 };

    // Threshold operations
    int threshold_type{ 0 };
    int threshold_min_value{ 0 };
    int threshold_max_value{ 255 };
    int steps{ 255 };
};
// Global variables
cv::Mat src, gray, thresholded;
int threshold_max_value = 255;
int threshold_min_value = 0;
int max_value = 255;
int threshold_type = 0;

// Callback function for the trackbar
void Threshold_Demo(int, void*) {
    // Apply the selected thresholding method
    cv::threshold(gray, thresholded, threshold_min_value, threshold_max_value, threshold_type);

    // Display the result
    cv::imshow("Thresholded Image", thresholded);
}

void ProcessImage(int, void* data) {
  auto* ud = static_cast<UserData*>(data);
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


    // Load the image
    src = img.clone();

    // Convert to grayscale
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // Create a window
    cv::namedWindow("Thresholded Image", cv::WINDOW_AUTOSIZE);

    // Create trackbars
    cv::createTrackbar("Threshold Max Value", "Thresholded Image", &threshold_max_value, 255, Threshold_Demo);
    cv::createTrackbar("Threshold Min Value", "Thresholded Image", &threshold_min_value, 255, Threshold_Demo);
    cv::createTrackbar("Threshold Type", "Thresholded Image", &threshold_type, 4, Threshold_Demo);

    // Show the original image
    cv::imshow("Original Image", src);

    // Call the callback function once to initialize
    Threshold_Demo(0, 0);

    // Wait for a key press indefinitely
    cv::waitKey(0);

    return 0;


    return 0;
}
