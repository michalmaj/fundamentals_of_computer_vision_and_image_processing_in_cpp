#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <vector>
#include <xtensor/xtensor.hpp>
#include <xtensor/xmath.hpp>



int main() {
    // Path to an image 
    std::string path{ "../data/images/girl.jpg" };

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

    xt::xtensor<float, 1> original_value{ 0,50,100,150,200,255 };
    xt::xtensor<float, 1> bcurve{ 0,80,150,190,220,255 };
    xt::xtensor<float, 1> rcurve{ 0,20,40,75,150,255 };

    xt::xtensor<float, 1> full_range = xt::arange<float>(0, 256);
    auto result = xt::interp(full_range, original_value, bcurve);

    std::vector<uchar> vec{ result.begin(), result.end() };
    cv::Mat lut = cv::Mat(vec, true).reshape(1, 1);

    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    cv::Mat blut;
    cv::LUT(channels.at(0), lut, blut);

    result = xt::interp(full_range, original_value, rcurve);
    vec = { result.begin(), result.end() };
    lut = cv::Mat(vec, true).reshape(1, 1);
    cv::Mat rlut;
    cv::LUT(channels.at(2), lut, rlut);

    cv::Mat output;
    std::vector<cv::Mat> modified_channels{ blut, channels.at(1), rlut };
    cv::merge(modified_channels, output);

    cv::imshow("Original", img);
    cv::imshow("LUT", output);
    cv::waitKey(0);
    cv::destroyAllWindows();


    return 0;
}
