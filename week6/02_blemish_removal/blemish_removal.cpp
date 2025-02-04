#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <stdexcept>
#include <format>
#include <filesystem>
#include <array>

class BlemishRemoval {
public:
    BlemishRemoval(const std::filesystem::path& path) : img_(cv::imread(path.string())) {
        if (img_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path.string()));
        }
        copy_ = img_.clone();
    }

    // Get images
    const auto& getImg() const { return img_; }
    const auto& getProcessed() const { return processed_; }
    auto& getRoi() { return roi_; }

    // Get window names
    const auto& getOriginalWindowName() const { return original_window_name_; }
    const auto& getProcessedWindowName() const { return processed_window_name_; }
    const auto& getSkinAreaWindowName() const { return skin_area_window_name_; }

    // Get patch
    int getMaxPatch() const {
        return max_patches_;
    }

    int& getCurrentIndex() {
        return current_index_;
    }

    int getDirection(size_t idx) const {
        return pixels_each_directions_.at(idx);
    }

    auto getTopLeft() const {
        return tl_;
    }

    // Set images
    void setProcessed(cv::Mat&& processed) {
        processed_ = std::move(processed);
    }

    // Set Points
    void setMiddle(cv::Point point) {
        middle_ = point;
    }

    void setTlWidthHeight(cv::Point middle) {
        tl_.x = std::clamp(middle.x - getDirection(getCurrentIndex()), 0, img_.size().width);
        tl_.y = std::clamp(middle.y - getDirection(getCurrentIndex()), 0, img_.size().height);
        int br_x = std::min(middle.x + getDirection(getCurrentIndex()), img_.cols);
        int br_y = std::min(middle.y + getDirection(getCurrentIndex()), img_.rows);

        width_ = br_x - tl_.x;
        height_ = br_y - tl_.y;
    }

    void setRoi() {
        roi_ = img_(cv::Range(tl_.y, tl_.y + height_), cv::Range(tl_.x, tl_.x + width_));
    }

    void porcelainSkin() {
        try {
            //cv::bilateralFilter(roi_, processed_, 9, 50, 50);
            cv::medianBlur(roi_, processed_, 5);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            std::cout << "ROI type: " << roi_.type() << std::endl;
        }
        processed_.copyTo(img_(cv::Range(tl_.y, tl_.y + height_), cv::Range(tl_.x, tl_.x + width_)));
    }

private:
    // Images
    cv::Mat img_;
    cv::Mat copy_;
    cv::Mat processed_;
    cv::Mat roi_;

    // Window names
    std::string original_window_name_{ "Original Image" };
    std::string processed_window_name_{ "Processed" };
    std::string skin_area_window_name_{ "Select Skin Area" };

    // Patch settings
    static constexpr int max_patches_{ 9 };
    static constexpr std::array<int, max_patches_> pixels_each_directions_{ 3, 5, 7, 9, 11, 13, 15, 17, 21 };
    int current_index_{ 0 };
    cv::Point middle_{ -1, -1 };
    cv::Point tl_{};
    int width_{};
    int height_{};
};

void getPatch(int event, int x, int y, int flags, void* data) {
    BlemishRemoval* br = static_cast<BlemishRemoval*>(data);

    if (event == cv::EVENT_LBUTTONDOWN) {
        br->setTlWidthHeight({ x, y });
    }
    else if (event == cv::EVENT_LBUTTONUP) {
        br->setRoi();
        cv::imshow(br->getSkinAreaWindowName(), br->getRoi());
    }
}

// TODO: implement Porcelain Skin algorithm

int main() {
    std::filesystem::path path{ "../data/images/blemish.png" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't load an image from: {}", path.string());
        return EXIT_FAILURE;
    }

    BlemishRemoval br{ path };

    // Create named windows
    cv::namedWindow(br.getOriginalWindowName(), cv::WINDOW_NORMAL);

    // Create trackbar
    cv::createTrackbar("Patch", br.getOriginalWindowName(), &br.getCurrentIndex(), br.getMaxPatch() - 1);

    // Create mouse event
    cv::setMouseCallback(br.getOriginalWindowName(), getPatch, &br);

    while (true) {
        auto c = cv::waitKey(1);

        if (c == 'q') {
            break;
        }

        if (c == 'p') {
            br.porcelainSkin();
        }

        cv::imshow(br.getOriginalWindowName(), br.getImg());
    }

    cv::destroyAllWindows();
    return 0;
}
