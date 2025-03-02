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
        auto expected = checkCascade();
        if (!expected) {
            throw std::runtime_error(expected.error());
        }
        return expected.value();
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

class CascadeClassifierDetector {
public:
    CascadeClassifierDetector(const std::filesystem::path& path) {
        classifier_.load(path);
        setParams();
    }
    void setParams(double scale_factor = 1.2, int min_neighbors = 9) {
        scale_factor_ = scale_factor;
        min_neighbors_ = min_neighbors;
    }

    [[nodiscard]] cv::Mat getImageWithDetection(const cv::Mat& img) const {
        cv::Mat output = img.clone();
        auto expected = convertToGray(img);
        if (!expected) {
            throw std::runtime_error(expected.error());
        }
        auto gray = expected.value();
        std::vector<cv::Rect> faces;
        classifier_.getCascade().detectMultiScale(gray, faces, scale_factor_, min_neighbors_);

        for (auto const& face : faces) {
            int x = face.x;
            int y = face.y;
            int width = face.width;
            int height = face.height;

            cv::rectangle(output, cv::Point(x, y), cv::Point(x + width, y + height), cv::Scalar(255, 0, 255), 3);
        }
        return output;
    }
protected:
    LoadCascade classifier_;
    double scale_factor_{};
    int min_neighbors_{};

    std::expected<cv::Mat, std::string> convertToGray(const cv::Mat& img) const {
        if (img.empty()) {
            return std::unexpected("Your image is empty!\n");
        }
        cv::Mat gray;
        if (img.channels() == 1) {
            gray = img.clone();
        }
        else if (img.channels() == 3) {
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        }
        else if (img.channels() == 4) {
            cv::cvtColor(img, gray, cv::COLOR_BGRA2GRAY);
        }
        else {
            return std::unexpected("Your images has wrong number of channels!\n");
        }
        return gray;
    }
};


int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::println(std::cerr, "Can't open camera stream!");
        return EXIT_FAILURE;
    }

    CascadeClassifierDetector face_detector{ "../data/models/haarcascade_frontalface_default.xml" };
    CascadeClassifierDetector smile_detector{ "../data/models/haarcascade_smile.xml" };
    smile_detector.setParams(1.4, 9);

    while (true) {
        cv::Mat frame;
        cap.read(frame);

        if (frame.empty()) {
            std::println(std::cerr, "Frame is empty!");
            break;
        }

        auto face = face_detector.getImageWithDetection(frame);
        if (!face.empty()) {
            cv::imshow("Face detected", face);
        }

        auto smile = smile_detector.getImageWithDetection(frame);
        if (!smile.empty()) {
            cv::imshow("Smile detected", smile);
        }

        cv::imshow("Frame", frame);
        auto c = cv::waitKey(1);

        if (c == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
