#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <format>
#include <vector>


class SeamlessCloning {
public:
    SeamlessCloning(const std::string& src_path, const std::string& dst_path, std::vector<std::vector<cv::Point>>&& poly, cv::Point center) :
        src_(cv::imread(src_path)), dst_(cv::imread(dst_path)), poly_(std::move(poly)), center_(center) {
        if (src_.empty()) {
            throw std::runtime_error(std::format("Can't load source image from: {}", src_path));
        }

        if (dst_.empty()) {
            throw std::runtime_error(std::format("Can't load destination image from: {}", dst_path));
        }

        createMask();
    }

    SeamlessCloning(const std::string& src_path, const std::string& dst_path) :
        src_(cv::imread(src_path)), dst_(cv::imread(dst_path)) {
        if (src_.empty()) {
            throw std::runtime_error(std::format("Can't load source image from: {}", src_path));
        }

        if (dst_.empty()) {
            throw std::runtime_error(std::format("Can't load destination image from: {}", dst_path));
        }

        mask_ = 255 * cv::Mat::ones(src_.size(), src_.depth());
        center_ = cv::Point(dst_.cols / 2, dst_.rows / 2);
    }

    [[nodiscard]] cv::Mat process(int flag = cv::NORMAL_CLONE) const {
        cv::Mat output_;
        cv::seamlessClone(src_, dst_, mask_, center_, output_, flag);
        return output_;
    }

private:
    cv::Mat src_;
    cv::Mat dst_;
    cv::Mat mask_;
    cv::Point center_;
    std::vector<std::vector<cv::Point>> poly_;

    void createMask() {
        mask_ = cv::Mat::zeros(src_.size(), src_.depth());
        cv::fillPoly(mask_, poly_, cv::Scalar(255, 255, 255));
    }
};

int main() {
    SeamlessCloning sc_plane("../data/images/airplane.jpg",
        "../data/images/sky.jpg", {
            {
              cv::Point(4, 80), cv::Point(40, 54), cv::Point(151, 63), cv::Point(254,37),
              cv::Point(298,90), cv::Point(272,134), cv::Point(43,122)
            } },
        { 800, 100 });

    auto normal_clone_plane = sc_plane.process(cv::NORMAL_CLONE);
    auto mixed_clone_plane = sc_plane.process(cv::MIXED_CLONE);

    cv::imshow("Normal Seamless Cloning Plane", normal_clone_plane);
    cv::imshow("Mixed Seamless Cloning Plane", mixed_clone_plane);

    SeamlessCloning sc_text("../data/images/iloveyouticket.jpg", "../data/images/wood-texture.jpg");

    auto normal_clone_text = sc_text.process(cv::NORMAL_CLONE);
    auto mixed_clone_text = sc_text.process(cv::MIXED_CLONE);

    cv::imshow("Normal Seamless Cloning Text", normal_clone_text);
    cv::imshow("Mixed Seamless Cloning Text", mixed_clone_text);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
