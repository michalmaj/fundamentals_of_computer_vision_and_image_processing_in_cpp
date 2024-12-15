#include <matplot/matplot.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

void imShow(const std::string& path) {
    auto image{ matplot::imread(path) };
    matplot::imshow(image);
    matplot::show();
}

int main() {

    // Check image (x,y)-coordinates for roi purpose
    std::string path{ "../data/images/boy.jpg" };
    imShow(path);

    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string headRoi{ "ROI" };
    cv::namedWindow(headRoi, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_COLOR loads the image as a BGR image.
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Error: Cannot open the file {}\n", path));
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Get roi of head - given after image analyze in matplot++
    auto height = cv::Range(37, 194);
    auto width = cv::Range(173, 313);

    cv::Mat roi = img(height, width).clone();

    cv::imshow(original, img);
    cv::imshow(headRoi, roi);
    cv::waitKey(0);
    cv::destroyAllWindows();

}
