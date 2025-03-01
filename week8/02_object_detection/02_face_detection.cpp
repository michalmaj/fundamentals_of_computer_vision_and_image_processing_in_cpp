#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <filesystem>
#include <expected>

class LoadCascade {
public:
    void load(const std::filesystem::path& path) {
        auto expected = checkPath(path);
        if (!expected) {
            throw std::runtime_error(expected.error());
        }
        if (!cascade_.load(path.string())) {
            throw std::runtime_error("Can't load cascade!\n");
        }
    }

    auto getCascade() const {
        return checkCascade();
    }

private:
    cv::CascadeClassifier cascade_;

    std::expected<std::filesystem::path, std::string> checkPath(const std::filesystem::path& path) const {
        if (!std::filesystem::is_regular_file(path)) {
            return std::unexpected{ std::format("Given path: {} is not a file!", path.string()) };
        }
        if (path.extension().string() != ".xml") {
            return std::unexpected{ std::format(
                "Given file: {} has wrong format: {}, expected is .xml",
                path.string(),
                path.extension().string()) };
        }
        return path;
    }

    std::expected<cv::CascadeClassifier, std::string> checkCascade() const {
        if (cascade_.empty()) {
            return std::unexpected{ "Cascade is empty, first use load() method!\n" };
        }
        return cascade_;
    }
};


int main() {

    LoadCascade loader;
    loader.load("../data/models/haarcascade_frontalface_default.xml");
    auto expected = loader.getCascade();
    if (expected.has_value()) {
        std::cout << "FINE!\n";
    }

    return 0;
}
