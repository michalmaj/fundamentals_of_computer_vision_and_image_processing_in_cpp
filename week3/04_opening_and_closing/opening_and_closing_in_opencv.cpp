#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/opening.png" };
    auto opening{ cv::imread(path) };

    // Path to second image
    path = "../data/images/closing.png";
    auto closing{ cv::imread(path) };

    // Check if both images are present
    try {
        if (opening.empty() or closing.empty()) {
            throw std::runtime_error("Can't load images!\n");
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }

    // Set kernel size
    int kernel_size{ 10 };

    // Create structuring element
    auto element{ cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(2 * kernel_size + 1, 2 * kernel_size + 1),
        cv::Point(kernel_size, kernel_size)) };

    // 1. Wipe out small white blobs

    // Perform erosion
    cv::Mat eroded;
    cv::erode(opening, eroded, element);

    // Perform dilation on eroded image
    cv::Mat eroded_dilated;
    cv::dilate(eroded, eroded_dilated, element);

    // Perform opening
    cv::Mat opened;
    cv::morphologyEx(opening, opened, cv::MORPH_OPEN, element);

    // 2. Wipe out small black blobs

    // Perform dilatation
    cv::Mat dilated;
    cv::dilate(closing, dilated, element);

    // Perform erosion on dilated image
    cv::Mat dilated_eroded;
    cv::erode(dilated, dilated_eroded, element);

    // Perform closing
    cv::Mat closed;
    cv::morphologyEx(closing, closed, cv::MORPH_CLOSE, element);


    cv::imshow("Original Opening", opening);
    cv::imshow("Original Closing", closing);
    cv::imshow("After erosion", eroded);
    cv::imshow("After Erosion and Dilation", eroded_dilated);
    cv::imshow("Opened", opened);
    cv::imshow("After dilation", dilated);
    cv::imshow("After Dilation and Erosion", dilated_eroded);
    cv::imshow("Closed", closed);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
