#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <matplot/matplot.h>
#include <vector>


/// <summary>
/// Function for histogram comparision
/// </summary>
/// <param name="gray1">First image in grayscale</param>
/// <param name="gray2">Second image in grayscale</param>
void compareHistograms(const cv::Mat& gray1, const cv::Mat& gray2) {
    // Check if both images are in grayscale
    if (gray1.channels() != 1 and gray2.channels() != 1) {
        throw std::invalid_argument("Only grayscale images accepted!");
    }

    // Create a distribution of values from 0  to 255
    std::vector<uchar> distribution1(gray1.begin<uchar>(), gray1.end<uchar>());
    std::vector<uchar> distribution2(gray2.begin<uchar>(), gray2.end<uchar>());

    // Create subplot with 1 row and 2 columns
    matplot::subplot(1, 2, 0);
    matplot::hist(distribution1, matplot::histogram::binning_algorithm::automatic);
    matplot::subplot(1, 2, 1);
    matplot::hist(distribution2, matplot::histogram::binning_algorithm::automatic);
    matplot::show();
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/dark-flowers.jpg" };

    // Load the image
    cv::Mat img{ cv::imread(path) };

    // Check if the image is loaded correctly
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Convert image to gray scale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Equalize the histogram of grayscale image
    cv::Mat gray_equalize;
    cv::equalizeHist(gray, gray_equalize);

    // Compare both histograms
    compareHistograms(gray, gray_equalize);

    // Equalize histograms for BGR image
    // Convert image to hsv colorspace
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // Split channels of hsv image
    std::vector<cv::Mat> hsv_channels(3);
    cv::split(hsv, hsv_channels);

    // Equalize only value channel
    cv::equalizeHist(hsv_channels.at(2), hsv_channels.at(2));

    // Merge back to hsv Mat
    cv::merge(hsv_channels, hsv);

    // Create new bgr image with equalized histogram
    cv::Mat equalized_img;
    cv::cvtColor(hsv, equalized_img, cv::COLOR_HSV2BGR);

    cv::imshow("Gray", gray);
    cv::imshow("Gray Equalize", gray_equalize);
    cv::imshow("Original image", img);
    cv::imshow("BGR Equalized", equalized_img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
