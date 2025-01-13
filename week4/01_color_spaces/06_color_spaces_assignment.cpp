#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <array>
#include <random>
#include <print>
#include <stdexcept>
#include <matplot/matplot.h>
#include <vector>
#include <ranges>
#include <execution>

auto minMaxColor(float b, float g, float r) {
    std::array<float, 3> colors{ b, g, r };
    auto min_max = std::ranges::minmax_element(colors);
    auto distance = std::distance(colors.begin(), min_max.max);
    char color{ 'b' };
    if (distance == 1) {
        color = 'g';
    }
    if (distance == 2) {
        color = 'r';
    }

    auto delta = *min_max.max - *min_max.min;

    return std::make_tuple(*min_max.min, *min_max.max, color, delta);
}

auto calculateHue(float b, float g, float r, char c, float delta) {
    if (delta == 0.0f) {
        return 0.0f;
    }
    float h{ 60.0f * ((r - g) / delta + 4) };
    if (c == 'r') {
        h = 60.0f * std::fmod((g - b) / delta, 6);
    }

    if (c == 'g') {
        h = 60.0f * ((b - r) / delta + 2);
    }

    if (h < 0) {
        h += 360.0f;
    }

    return h;
}

auto calculateSaturation(float cmax, float delta) {
    if (cmax == 0.0f) {
        return 0.0f;
    }

    return delta / cmax;
}

cv::Mat convertBGRtoHSV(cv::Mat img) {
    img.convertTo(img, CV_32FC3, 1.0 / 255.0);
    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    std::vector<float> blue{ channels.at(0).begin<float>(), channels.at(0).end<float>() };
    std::vector<float> green{ channels.at(1).begin<float>(), channels.at(1).end<float>() };
    std::vector<float> red{ channels.at(2).begin<float>(), channels.at(2).end<float>() };

    std::vector<float> hue;
    hue.reserve(blue.size());
    std::vector<float> saturation;
    saturation.reserve(blue.size());
    std::vector<float> value;
    value.reserve(blue.size());

    auto view = std::views::zip(blue, green, red);

    std::ranges::for_each(view, [&](auto&& tuple) {
        auto&& [b, g, r] = tuple;
        auto [cmin, cmax, maxC, delta] = minMaxColor(b, g, r);
        hue.emplace_back(calculateHue(b, g, r, maxC, delta) / 2.0f);
        saturation.emplace_back(calculateSaturation(cmax, delta));
        value.emplace_back(cmax);

        /*std::println("hue: {}, saturation: {}, value: {}",
            hue.back(), saturation.back(), value.back());*/
        });

    cv::Mat h(img.size(), CV_32F, hue.data());
    h.convertTo(h, CV_8U);
    cv::Mat s(img.size(), CV_32F, saturation.data());
    s.convertTo(s, CV_8U, 255.0);
    cv::Mat v(img.size(), CV_32F, value.data());
    v.convertTo(v, CV_8U, 255.0);

    std::vector<cv::Mat> hue_channels{
        h, s, v
    };
    cv::Mat hsv;
    cv::merge(hue_channels, hsv);


    return hsv;
}

cv::Mat convertBGRtoGray(const cv::Mat& img) {
    cv::Mat gray(img.size(), CV_8U);
    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    std::vector<uchar> blue{ channels.at(0).begin<uchar>(), channels.at(0).end<uchar>() };
    std::vector<uchar> green{ channels.at(1).begin<uchar>(), channels.at(1).end<uchar>() };
    std::vector<uchar> red{ channels.at(2).begin<uchar>(), channels.at(2).end<uchar>() };
    std::vector<uchar> output(blue.size());

    auto view = std::views::zip(blue, green, red, output);

    std::for_each(std::execution::par, view.begin(), view.end(), [](auto&& tuple) {
        auto&& [b, g, r, o] = tuple;
        o = 0.299 * r + 0.587 * g + 0.114 * b;
        });

    std::copy(output.begin(), output.end(), gray.begin<uchar>());
    gray.convertTo(gray, CV_8U);

    return gray;
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/girl.jpg" };

    cv::Mat img{ cv::imread(path) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    auto my_gray = convertBGRtoGray(img);
    cv::Mat real_gray;
    cv::cvtColor(img, real_gray, cv::COLOR_BGR2GRAY);

    auto my_hsv = convertBGRtoHSV(img);
    cv::Mat real_hsv;
    cv::cvtColor(img, real_hsv, cv::COLOR_BGR2HSV);

    cv::imshow("Original", img);
    cv::imshow("My Gray", my_gray);
    cv::imshow("Real Gray", real_gray);
    cv::imshow("My HSV", my_hsv);
    cv::imshow("Real HSV", real_hsv);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
