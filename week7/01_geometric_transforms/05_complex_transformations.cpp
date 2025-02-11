#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <array>
#include <numbers>
#include <cmath>

float degreeToRadians(float angle) {
    return angle * std::numbers::pi / 180.0f;
}

cv::Point calculateCenter(const cv::Mat& img) {
    cv::Mat gray = img.clone();
    if (gray.channels() != 1) {
        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
    }

    auto moment = cv::moments(gray);

    if (moment.m00 == 0) {
        throw std::runtime_error("Can't divide by 0");
    }

    return { static_cast<int>(moment.m10 / moment.m00), static_cast<int>(moment.m01 / moment.m00) };
}

cv::Mat rotatedImage(const cv::Mat& img, float angle, bool my_center = false, cv::Size size = {}) {
    if (img.empty()) {
        throw std::runtime_error("Image is empty\n");
    }

    if (size.empty()) {
        size = img.size();
    }
    cv::Point center{};
    float tx{ 0.0 };
    float ty{ 0.0 };
    angle = degreeToRadians(angle);
    float cosTheta = std::cos(angle);
    float sinTheta = std::sin(angle);

    if (my_center) {
        center = calculateCenter(img);
        tx = (1 - cosTheta) * center.x - sinTheta * center.y;
        ty = sinTheta * center.x + (1 - cosTheta) * center.y;
    }

    std::array<float, 6> warp_values{ cosTheta, sinTheta, tx, -sinTheta, cosTheta, ty };
    cv::Mat warp_mat(2, 3, CV_32F, warp_values.data());

    cv::Mat rotated;
    cv::warpAffine(img, rotated, warp_mat, size);

    return rotated;
}

cv::Mat rotatedImageCV(const cv::Mat& img, float angle, bool my_center = false, cv::Size size = {}) {
    if (img.empty()) {
        throw std::runtime_error("Image is empty\n");
    }

    if (size.empty()) {
        size = img.size();
    }
    cv::Point center{};
    float tx{ 0.0 };
    float ty{ 0.0 };

    if (my_center) {
        center = calculateCenter(img);
        tx = center.x;
        ty = center.y;
    }

    cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point2f(tx, ty), angle, 1);

    cv::Mat rotated;
    cv::warpAffine(img, rotated, rotation_matrix, size);

    return rotated;
}

class AffineTransform {
public:
    AffineTransform(const cv::Mat& img) : img_(img) {
        size_ = img.size();
        matrices_.reserve(3);
    }

    void scaleImage(float scale_x = 1.0f, float scale_y = 1.0f, bool rescaled = true) {
        matrices_.emplace_back((cv::Mat_<float>(2, 2) << scale_x, 0.0f, 0.0f, scale_y));
        if (rescaled) {
            size_.height *= static_cast<int>(scale_y);
            size_.width *= static_cast<int>(scale_x);
        }
    }

    [[nodiscard]] cv::Mat process() {
        for (const auto& e : matrices_) {
            processed_ *= e;
        }
        cv::Mat warp_mat;
        cv::hconcat(processed_, translate_vector_, warp_mat);

        cv::Mat output;
        cv::warpAffine(img_, output, warp_mat, size_);
        return output;
    }

    void eraseOperations() {
        matrices_.clear();
        matrices_.reserve(3);
    }

    const auto& getImg() const { return img_; }
    const auto& getOriginalWindows() const { return original_window_; }
    const auto& getProcessedWindows() const { return processed_window_; }

private:
    cv::Mat img_;
    cv::Mat processed_{ (cv::Mat_<float>(2, 2) << 1.0f, 0.0f, 0.0f, 1.0f) };
    std::vector<cv::Mat> matrices_;

    cv::Size size_;

    cv::Mat translate_vector_{ (cv::Mat_<float>(2, 1) << 0.0f, 0.0f) };

    // Window names
    const std::string original_window_{ "Original image" };
    const std::string processed_window_{ "Processed image" };
};

int main() {
    // Create an empty black image
    cv::Mat img(300, 300, CV_32FC3, cv::Scalar(0, 0, 0));

    // Add a blue square in the middle
    int cx{ img.size().width / 2 };
    int cy{ img.size().height / 2 };
    cv::Point tl{ cx - cx / 2, cy - cy / 2 };
    img(cv::Range(tl.y, tl.y + cy), cv::Range(tl.x, tl.x + cx)).setTo(cv::Scalar(1, 0.6, 0.2));


    AffineTransform af{ img };
    af.scaleImage(2.0);
    auto processed = af.process();

    cv::imshow(af.getOriginalWindows(), af.getImg());
    cv::imshow(af.getProcessedWindows(), processed);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
