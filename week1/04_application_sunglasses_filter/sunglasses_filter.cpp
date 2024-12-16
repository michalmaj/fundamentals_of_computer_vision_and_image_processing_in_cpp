#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <ranges>

std::optional<cv::Mat> loadImage(const std::string& path, int flags = cv::IMREAD_COLOR) {
    cv::Mat img{ cv::imread(path, flags) };
    if (img.empty()) {
        return std::nullopt;
    }

    return img;
}

std::pair<cv::Mat, cv::Mat> splitBGRAlpha(const cv::Mat& mat) {
    if (mat.channels() != 4) {
        throw std::runtime_error("Mat object should have four channels!\n");
    }

    // Split into four channels
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);

    // Get alpha channel
    cv::Mat alpha = channels.at(3);
    channels.pop_back();

    // Get bgr image
    cv::Mat bgr;
    cv::merge(channels, bgr);

    return std::pair<cv::Mat, cv::Mat>{bgr, alpha};

}

void maskImageError(const cv::Mat& mask, const cv::Mat& image) {
    if (mask.size() != image.size() or mask.channels() != image.channels()) {
        throw std::range_error("Wrong format of image and mask, must be same size and channels!\n");
    }
}

cv::Mat maskImage(const cv::Mat& mask, const cv::Mat& image, const std::string& type = "add") {
    // Split both mask and image into separate channels
    std::vector<cv::Mat> src1;
    cv::split(mask, src1);
    std::vector<cv::Mat> src2;
    cv::split(image, src2);

    // Create vector for output result
    std::vector<cv::Mat> dst(src1.size());

    // Create a view of three vectors
    auto view = std::views::zip(src1, src2, dst);

    // Perform a loop for all elements inside given view
    for (auto&& [s1, s2, d] : view) {
        if (type == "add") {
            cv::add(s1, s2, d);
        }
        else if (type == "mul") {
            cv::multiply(s1, s2, d);
        }
        else {
            throw std::runtime_error("Invalid operation");
        }
    }

    cv::Mat output;
    cv::merge(dst, output);

    return output;
}

cv::Mat multiplyMaskImage(const cv::Mat& mask, const cv::Mat& image) {
    maskImageError(mask, image);

    return maskImage(mask, image, "mul");
}

cv::Mat addMaskImage(const cv::Mat& mask, const cv::Mat& image) {
    maskImageError(mask, image);

    return maskImage(mask, image, "add");
}

int main() {

    // Load image of Elon Musk
    auto check = loadImage("../data/images/musk.jpg");
    if (!check.has_value()) {
        std::cerr << "Image with Elon Musk can't be loaded\n";
        return EXIT_FAILURE;
    }
    auto musk = check.value();
    // Normalize image into 0-1 range
    cv::normalize(musk, musk, 0, 1, cv::NORM_MINMAX, CV_32FC3);

    // Create a copy
    auto img = musk.clone();

    // Load glasses
    check = loadImage("../data/images/sunglass.png", cv::IMREAD_UNCHANGED);
    if (!check.has_value()) {
        std::cerr << "Image with sunglasses can't be loaded\n";
        return EXIT_FAILURE;
    }
    auto bgr_alpha = check.value();
    // Resize sunglasses (given by tests)
    cv::resize(bgr_alpha, bgr_alpha, cv::Size(), 0.5, 0.5);

    // Split into bgr image and alpha mask
    auto [glasses, mask] = splitBGRAlpha(bgr_alpha);

    // Change black pixels into white and white into black (image must be 8U type!)
    cv::Mat reversed_mask;
    cv::bitwise_not(mask, reversed_mask);

    std::vector<cv::Mat> temp(3, mask);
    cv::merge(temp, mask);

    for (auto& e : temp) {
        e = reversed_mask;
    }

    cv::merge(temp, reversed_mask);

    // Normalize glasses and mask
    cv::normalize(glasses, glasses, 0, 1, cv::NORM_MINMAX, CV_32FC3);
    cv::normalize(reversed_mask, reversed_mask, 0, 1, cv::NORM_MINMAX, CV_32FC3);
    cv::normalize(mask, mask, 0, 1, cv::NORM_MINMAX, CV_32FC3);

    // Get ROI of eyes
    cv::Mat eye_roi = img(cv::Range(140, 140 + glasses.rows), cv::Range(130, 130 + glasses.cols));

    auto masked_eye = multiplyMaskImage(reversed_mask, eye_roi);
    auto masked_glasses = multiplyMaskImage(mask, glasses);

    auto eyes_with_glasses = addMaskImage(masked_glasses, masked_eye);

    eyes_with_glasses.copyTo(img(cv::Range(140, 140 + glasses.rows), cv::Range(130, 130 + glasses.cols)));

    cv::imshow("Roi", eye_roi);
    cv::imshow("Masked Eye", masked_eye);
    cv::imshow("Masked glasses", masked_glasses);
    cv::imshow("Eyes with glasses", eyes_with_glasses);
    cv::imshow("Musk", musk);
    cv::imshow("Glasses", glasses);
    cv::imshow("Mask", mask);
    cv::imshow("Musk with glasses", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
