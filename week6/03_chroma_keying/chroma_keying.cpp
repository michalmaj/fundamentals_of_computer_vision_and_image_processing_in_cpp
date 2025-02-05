#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <stdexcept>
#include <format>
#include <filesystem>
#include <vector>

class ColorPatchSelector {
public:
    void setLab(const cv::Mat& bgr) {
        if (bgr.type() != CV_8UC3) {
            throw std::runtime_error("Wrong image type!\n");
        }
        cv::cvtColor(bgr, lab_, cv::COLOR_BGR2Lab);
    }

    void addColor(cv::Point p) {
        colors_.push_back(lab_.at<cv::Vec3b>(p.y, p.x));
    }

    cv::Mat& getMask() {
        cv::Mat mask;
        cv::inRange(lab_, lower_, upper_, mask);
        return mask;
    }

private:
    cv::Mat lab_;
    std::vector<cv::Vec3b> colors_;
    cv::Scalar lower_{ 255, 255, 255 };
    cv::Scalar upper_{ 0, 0, 0 };

    void findLowerUpper() {
        std::ranges::for_each(colors_, [this](const auto& color) {
            lower_[0] = std::min(static_cast<int>(color[0]), static_cast<int>(lower_[0]));
            lower_[1] = std::min(static_cast<int>(color[1]), static_cast<int>(lower_[1]));
            lower_[2] = std::min(static_cast<int>(color[2]), static_cast<int>(lower_[2]));

            upper_[0] = std::max(static_cast<int>(color[0]), static_cast<int>(upper_[0]));
            upper_[1] = std::max(static_cast<int>(color[1]), static_cast<int>(upper_[1]));
            upper_[2] = std::max(static_cast<int>(color[2]), static_cast<int>(upper_[2]));
            });
    }
};

class BlueScreenMatting {
public:
    BlueScreenMatting(const std::filesystem::path& background_path) : background_(cv::imread(background_path.string())) {}

    auto& getImg() { return img_; }
    auto& getImageWindowName() const {
        return image_window_name_;
    }
private:
    cv::Mat img_;
    cv::Mat background_;
    ColorPatchSelector cps_;

    const std::string image_window_name_{ "Original Image" };
};

int main() {
    cv::VideoCapture cap;
    std::filesystem::path video_path{ "../data/videos/greenscreen-demo.mp4" };
    cap.open(video_path.string());
    if (!cap.isOpened()) {
        std::cerr << std::format("Can't load video from: {}\n", video_path.string());
        return EXIT_FAILURE;
    }

    std::filesystem::path path{ "../data/images/IF4.jpg" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't load an image from: {}", path.string());
        return EXIT_FAILURE;
    }
    BlueScreenMatting bsm{ path };

    cv::namedWindow(bsm.getImageWindowName(), cv::WINDOW_AUTOSIZE);

    while (true) {
        cap.read(bsm.getImg());
        if (bsm.getImg().empty()) {
            break;
        }
        auto c = cv::waitKey(100);
        if (c == 'q') {
            break;
        }

        cv::imshow(bsm.getImageWindowName(), bsm.getImg());
    }

    cap.release();
    cv::destroyAllWindows();


    return 0;
}
