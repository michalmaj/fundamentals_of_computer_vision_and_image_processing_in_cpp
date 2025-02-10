#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <array>

cv::Mat scaleImage(const cv::Mat& img, float a = 1.0f, float d = 1.0f, cv::Size size = {}) {
    if (img.empty()) {
        throw std::invalid_argument("Input image is empty!");
    }

    if (size.empty()) {
        size = cv::Size(static_cast<int>(img.cols * a), static_cast<int>(img.rows * d));
    }

    cv::Mat warp_mat = (cv::Mat_<float>(2, 3) << a, 0, 0, 0, d, 0);
    cv::Mat scaled;
    cv::warpAffine(img, scaled, warp_mat, size);

    return scaled;
}


int main() {
    // Create an empty black image
    cv::Mat img(300, 300, CV_32FC3, cv::Scalar(0, 0, 0));

    // Add a blue square in the middle
    int cx{ img.size().width / 2 };
    int cy{ img.size().height / 2 };
    cv::Point tl{ cx - cx / 2, cy - cy / 2 };
    img(cv::Range(tl.y, tl.y + cy), cv::Range(tl.x, tl.x + cx)).setTo(cv::Scalar(1, 0.6, 0.2));

    auto scaled = scaleImage(img, 2.0);

    cv::imshow("Original", img);
    cv::imshow("Scalede Image", scaled);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
