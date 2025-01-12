#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <print>
#include <stdexcept>
#include <matplot/matplot.h>
#include <vector>

class DesaturationFilter {
public:
    void load(const std::string& path) {
        img_ = cv::imread(path);
        if (img_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
        convertSplitNormalize();
    }

    auto& getImg() { return img_; }
    auto& getHsv() { return hsv_; }
    auto& getChannels() { return channels_; }
    auto& getProcessed() { return processed_; }
    auto& getModified() { return modified_; }
    auto& getWindowName() { return window_name_; }
    auto& getTrackbarName() { return trackbar_name_; }
    auto& getSaturationStart() { return saturation_start_; }
    const auto& getImg()const { return img_; }
    const auto& getHsv() const { return hsv_; }
    const auto& getChannels() const { return channels_; }
    const auto& getProcessed() const { return processed_; }
    const auto& getModified() const { return modified_; }
    const  auto& getWindowName() const { return window_name_; }
    const auto& getTrackbarName() const { return trackbar_name_; }

    auto& getSaturationSteps() { return saturation_steps_; }

    void process() {
        modified_.at(1).convertTo(modified_.at(1), CV_8U);
        cv::merge(modified_, hsv_);
        cv::cvtColor(hsv_, processed_, cv::COLOR_HSV2BGR);
    }

private:
    cv::Mat img_;
    cv::Mat hsv_;
    cv::Mat processed_;
    std::vector<cv::Mat> channels_;
    std::vector<cv::Mat> modified_;
    std::string window_name_{ "Processed" };
    std::string trackbar_name_{ "Saturation" };
    int saturation_start_{ 0 };
    int saturation_steps_{ 100 };

    void convertSplitNormalize() {
        cv::cvtColor(img_, hsv_, cv::COLOR_BGR2HSV);
        cv::split(hsv_, channels_);
        channels_.at(1).convertTo(channels_.at(1), CV_32F);

        for (const auto& e : channels_) {
            modified_.emplace_back(e.clone());
        }
    }
};

void desaturation(int, void* data) {
    auto* df = static_cast<DesaturationFilter*>(data);
    float scale = static_cast<float>(df->getSaturationStart()) / df->getSaturationSteps();
    df->getModified().at(1) = df->getChannels().at(1).clone() * scale;
    df->process();
    cv::imshow(df->getWindowName(), df->getProcessed());
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/girl.jpg" };

    // Load an image
    DesaturationFilter df;
    // Check if image is loaded
    try {
        df.load(path);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    cv::namedWindow(df.getWindowName(), cv::WINDOW_AUTOSIZE);

    cv::createTrackbar(df.getTrackbarName(), df.getWindowName(), &df.getSaturationStart(), df.getSaturationSteps(), desaturation, &df);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
