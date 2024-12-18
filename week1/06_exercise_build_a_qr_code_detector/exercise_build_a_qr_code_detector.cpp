// QR Code Detector
// Outline:
// Read image
// Detect QR Code in the image
// Draw bounding box around the detected QR Code
// Print the decoded text
// Save and display the result image

// Step 0: Include Libraries
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdexcept>
#include <print>
#include <vector>


int main() {
    // Step 1: Read Image
    // Path to an image 
    std::string path{ "../data/images/IDCard-Satya.png" };
    // Load the image
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    std::println("Image width, height, depth: [{}x{}x{}]", img.cols, img.rows, img.channels());

    std::string window_name{ "Image" };
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);

    // Step 2: Detect QR Code in the Image
    std::vector<cv::Point> bbox; // vector that will contain 4 points (index 0 top left, index 2 bottom right)
    cv::Mat rectifiedImage = img.clone();

    cv::QRCodeDetector qrDecoder;
    auto text = qrDecoder.detectAndDecode(img, bbox);
    try {
        if (bbox.size() != 4) {
            throw std::out_of_range("Wrong number of points\n");
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Step 3: Draw bounding box around the detected QR Code
    cv::rectangle(rectifiedImage, bbox.at(0), bbox.at(2), cv::Scalar(255, 0, 255), 3, cv::LINE_AA);

    // Step 4: Print the Decoded Text
    std::println("{}", text);

    // Step 5: Save and display the result image
    cv::imwrite("../data/images/AnnotatedIDCard-Satya.png", rectifiedImage);

    cv::imshow(window_name, rectifiedImage);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
