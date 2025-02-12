#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <array>
#include <numbers>
#include <cmath>

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

    void rotateImage(float angle, bool my_center = true) {
        angle = degreeToRadians(angle);
        float sinTheta{ std::sin(angle) };
        float cosTheta{ std::cos(angle) };

        if (my_center) {
            auto center{ calculateCenter() };
            translate_vector_.at<float>(0, 0) += (1 - cosTheta) * center.x - sinTheta * center.y;
            translate_vector_.at<float>(1, 0) += sinTheta * center.x + (1 - cosTheta) * center.y;
        }
        matrices_.emplace_back((cv::Mat_<float>(2, 2) << cosTheta, sinTheta, -sinTheta, cosTheta));
    }

    void shearImage(float shear_x = 0.0f, float shear_y = 0.0f) {
        matrices_.emplace_back((cv::Mat_<float>(2, 2) << 1.0f, shear_x, shear_y, 1.0f));
    }

    void translateImage(float translate_x = 0.0f, float translate_y = 0.0f) {
        translate_vector_.at<float>(0, 0) += translate_x;
        translate_vector_.at<float>(1, 0) += translate_y;
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

    void setHeight(int heignt) {
        size_.height *= heignt;
    }

    void setWidth(int width) {
        size_.width *= width;
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

    // Helper methods
    float degreeToRadians(float angle) const {
        return angle * std::numbers::pi / 180.0f;
    }

    cv::Point calculateCenter() const {
        cv::Mat gray = img_.clone();
        if (gray.channels() != 1) {
            cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
        }

        auto moment = cv::moments(gray);

        if (moment.m00 == 0) {
            throw std::runtime_error("Can't divide by 0");
        }

        return { static_cast<int>(moment.m10 / moment.m00), static_cast<int>(moment.m01 / moment.m00) };
    }
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
    af.scaleImage(1.1f, 1.1f);
    af.shearImage(-0.1f);
    af.rotateImage(10.0f);
    af.translateImage(10.0f, -10.0f);
    auto processed = af.process();

    cv::imshow(af.getOriginalWindows(), af.getImg());
    cv::imshow(af.getProcessedWindows(), processed);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
