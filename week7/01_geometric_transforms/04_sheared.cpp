#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <array>
#include <numbers>
#include <cmath>

cv::Mat shearImage(const cv::Mat& img, float shear_x = 0.0f, float shear_y = 0.0f, cv::Size size = {}) {
    if (img.empty()) {
        throw std::runtime_error("Image is empty!\n");
    }

    if (size.empty()) {
        size = img.size();
    }

    std::array<float, 6> warp_values{ 1.0f, shear_x, 0.0f, shear_y, 1.0f, 0.0f };
    cv::Mat warp_mat(2, 3, CV_32F, warp_values.data());

    cv::Mat sheared;
    cv::warpAffine(img, sheared, warp_mat, size);

    return sheared;
}

int main() {
    // Create an empty black image
    cv::Mat img(300, 300, CV_32FC3, cv::Scalar(0, 0, 0));

    // Add a blue square in the middle
    int cx{ img.size().width / 2 };
    int cy{ img.size().height / 2 };
    cv::Point tl{ cx - cx / 2, cy - cy / 2 };
    img(cv::Range(tl.y, tl.y + cy), cv::Range(tl.x, tl.x + cx)).setTo(cv::Scalar(1, 0.6, 0.2));

    auto sheared = shearImage(img, 0.1f, 0.2f);

    cv::imshow("Original", img);
    cv::imshow("Sheared Image", sheared);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
