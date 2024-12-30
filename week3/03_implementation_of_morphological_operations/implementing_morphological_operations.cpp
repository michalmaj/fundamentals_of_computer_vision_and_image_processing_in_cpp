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
    cv::Mat element{ cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)) };
    std::cout << element;
    std::cout << "\n\n";

    // Get info about size of th kernel - just one dimension, because structuring element has NxN shape
    auto ksize{ element.size().height };

    // Get size of the image
    auto [width, height] = demo.size();
    std::cout << width << ", " << height << "\n\n";

    // Set value of border
    int border{ ksize / 2 };

    cv::Mat paddle_demo{ cv::Mat::zeros(cv::Size(height + border * 2, width + border * 2), CV_8U) };
    cv::Mat bit_or;

    for (int h_i = border; h_i < height + border; h_i++) {
        for (int w_i = border; w_i < width + border; w_i++) {
          if (demo.at<uchar>(h_i - border, w_i - border)) {
              cv::bitwise_or(paddle_demo(
                  cv::Range(h_i - border, h_i + border + 1), cv::Range(w_i - border, w_i + border + 1)),
                  element, 
                  bit_or);
              bit_or.copyTo(paddle_demo(
                  cv::Range(h_i - border, h_i + border + 1), 
                  cv::Range(w_i - border, w_i + border + 1)));
          }
        }
    }

    cv::Mat dilated_image = paddle_demo(
        cv::Range(border, border + height), 
        cv::Range(border, border + width));

    std::string blobs{ "Blobs" };
    cv::namedWindow(blobs, cv::WINDOW_NORMAL);
    cv::imshow(blobs, demo * 255);

    std::string cross{ "Cross" };
    cv::namedWindow(cross, cv::WINDOW_NORMAL);
    cv::imshow(cross, element * 255);

    std::string dilated{ "Dilated" };
    cv::namedWindow(dilated, cv::WINDOW_NORMAL);
    cv::imshow(dilated, dilated_image * 255);


    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
