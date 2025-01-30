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
  BlemishRemoval(const std::filesystem::path& path): img_(cv::imread(path.string())) {}

private:
    cv::Mat img_;
};



int main() {
    std::filesystem::path path{ "../data/images/blemish.png" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't load an image from: {}", path.string());
        return EXIT_FAILURE;
    }
    

    BlemishRemoval br{path};

    return 0;
}
