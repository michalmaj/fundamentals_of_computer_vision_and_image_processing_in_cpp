#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <stdexcept>
#include <format>
#include <filesystem>

class BlemishRemoval {
public:
    BlemishRemoval(const std::filesystem::path& path) : img_(cv::imread(path.string())) {
        copy_ = img_.clone();
    }

    // Get image
    const auto& getImg() const { return img_; }
    const auto& getMask() const { return mask_; }

    // Get window names
    const auto& getOriginalWindowName() const { return original_window_name_; }


    // Set images
    void setMask(cv::Mat&& mask) {
        mask_ = std::move(mask);
    }

    // Methods

private:
    // Images
    cv::Mat img_;
    cv::Mat copy_;
    cv::Mat mask_;

    // Window names
    const std::string original_window_name_{ "Original Image" };
    const std::string mask_window_name{ "Mask" };
};


int main() {
    std::filesystem::path path{ "../data/images/blemish.png" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't load an image from: {}", path.string());
        return EXIT_FAILURE;
    }

    BlemishRemoval br{ path };

    // Create named windows
    cv::namedWindow(br.getOriginalWindowName(), cv::WINDOW_AUTOSIZE);

    while (true) {
        auto c = cv::waitKey(0);

        if (c == 'q') {
            break;
        }

        cv::imshow(br.getOriginalWindowName(), br.getImg());
    }

    cv::destroyAllWindows();
    return 0;
}
