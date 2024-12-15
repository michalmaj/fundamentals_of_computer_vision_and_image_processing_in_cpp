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
    //imShow(path);

    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string headRoi{ "ROI" };
    cv::namedWindow(headRoi, cv::WINDOW_NORMAL);

    std::string heads{ "Heads" };
    cv::namedWindow(heads, cv::WINDOW_NORMAL);

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

    // Paste roi into image copy
    cv::Mat copy = img.clone();

  // All location were picked by checking image in matplot++
    roi.copyTo(copy(cv::Range(37, 37 + roi.size().height), cv::Range(30, 30 + roi.size().width)));
    roi.copyTo(copy(cv::Range(37, 37 + roi.size().height), cv::Range(325, 325 + roi.size().width)));

    cv::imshow(original, img);
    cv::imshow(headRoi, roi);
    cv::imshow(heads, copy);
    cv::waitKey(0);
    cv::destroyAllWindows();

}
