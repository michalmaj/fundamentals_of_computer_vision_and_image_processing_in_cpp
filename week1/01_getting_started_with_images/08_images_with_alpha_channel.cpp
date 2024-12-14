#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <print>
#include <optional>

// Split 4-channels cv::Mat into bgr and alpha images
std::optional<std::pair<cv::Mat, cv::Mat>> splitAlpha(const cv::Mat& mat) {
    if (mat.channels() != 4) {
        std::cerr << "Error: Image does not have 4 channels.\n";
        return std::nullopt;
    }

    std::vector<cv::Mat> channels;
    channels.reserve(4);

    cv::split(mat, channels);

    auto alpha = channels.at(3).clone();
    channels.pop_back();

    cv::Mat bgr;
    cv::merge(channels, bgr);

    return std::pair<cv::Mat, cv::Mat>(bgr, alpha);
}

int main() {
    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string bgr{ "BGR" };
    cv::namedWindow(bgr, cv::WINDOW_NORMAL);

    std::string alpha{ "Alpha channel (mask)" };
    cv::namedWindow(alpha, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_UNCHANGED loads the image as a four-channels aBGR image (if possible).
    std::string path{ "../data/images/panther.png" };
    cv::Mat img{ cv::imread(path, cv::IMREAD_UNCHANGED) };

    // Check if the loaded image is valid
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Error: Cannot open the file {}\n", path));
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    std::println("Images dimension (WxHxD): [{}x{}x{}]", img.cols, img.rows, img.channels());

    // Checking if splitted images exist
    auto bgra = splitAlpha(img);
    if (bgra.has_value()) {
        auto [i, a] = bgra.value();

        cv::imshow(bgr, i);
        cv::imshow(alpha, a);
    }

    cv::imshow(original, img);
    cv::waitKey(0);
    cv::destroyAllWindows();

}
