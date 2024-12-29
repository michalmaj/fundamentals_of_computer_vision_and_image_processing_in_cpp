#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Create an empty (fill with 0) image, single channel with 10x10 size. 
    cv::Mat demo{ cv::Mat::zeros(cv::Size(10, 10),CV_8U) };
    std::cout << demo;
    std::cout << "\n\n";

    // Set some pixels to 1
    demo.at<uchar>(0, 1) = 1;
    demo.at<uchar>(9, 0) = 1;
    demo.at<uchar>(8, 9) = 1;
    demo.at<uchar>(2, 2) = 1;
    demo(cv::Range(5, 8), cv::Range(5, 8)).setTo(1);
    std::cout << demo;
    std::cout << "\n\n";

    // Create a structuring element
    cv::Mat cross_elemnt{ cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)) };
    std::cout << cross_elemnt;
    std::cout << "\n\n";

    std::string blobs{ "Blobs" };
    cv::namedWindow(blobs, cv::WINDOW_NORMAL);
    cv::imshow(blobs, demo * 255);

    std::string cross{ "Cross" };
    cv::namedWindow(cross, cv::WINDOW_NORMAL);
    cv::imshow(cross, cross_elemnt * 255);


    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
