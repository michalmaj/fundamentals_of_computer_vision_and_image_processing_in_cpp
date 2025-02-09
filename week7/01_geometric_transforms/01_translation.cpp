#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <array>

cv::Mat translateImage(const cv::Mat& img, float tx = 0.0f, float ty = 0.0f, cv::Size size = {}) {
    // Checking if cv::Size is passing, if not use img size instead
    if (size.width == 0 or size.height == 0) {
        size = img.size();
    }
    // Set warp values like matrix 2x3
    std::array<float, 6> warp_values{ 1.0f, 0.0f, tx, 0.0f, 1.0f, ty };
    cv::Mat warp_mat(2, 3, CV_32F, warp_values.data());

    // Do the translation
    cv::Mat translated;
    cv::warpAffine(img, translated, warp_mat, size);

    return translated;
}

int main() {
    // Create an empty black image
    cv::Mat img(300, 300, CV_32FC3, cv::Scalar(0, 0, 0));

    // Add a blue square in the middle
    int cx{ img.size().width / 2 };
    int cy{ img.size().height / 2 };
    cv::Point tl{ cx - cx / 2, cy - cy / 2 };
    img(cv::Range(tl.y, tl.y + cy), cv::Range(tl.x, tl.x + cx)).setTo(cv::Scalar(1, 0.6, 0.2));

    auto translated{ translateImage(img, 25, 25) };

    cv::imshow("Original", img);
    cv::imshow("Translate Image", translated);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
