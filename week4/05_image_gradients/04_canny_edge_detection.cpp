#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

class EdgeDetection {
public:
    EdgeDetection(const cv::Mat& mat) : img_(mat.clone()) {
        convertToGray();
    }

    // Get images
    auto& getImg() const { return img_; }
    auto& getBlurred() { return blurred_; }
    auto& getEdges() { return edges_; }

    // Get windows name
    auto& getWindowName() const { return window_name; }

    // Blur settings
    auto& getBlurAmount() { return blur_amount; }
    auto& getMaxBlurAmount() const { return max_blur_amount; }

    // Get aperture settings
    auto& getApertureSize() const { return aperture_size; }
    auto& getApertureIndex() { return aperture_index; }
    auto& getMaxApertureIndex() const { return max_aperture_index; }

    // Get threshold
    auto& getLowThreshold() { return low_threshold; }
    auto& getHighThreshold() { return high_threshold; }
    auto& getMaxThreshold() const { return max_threshold; }

private:
    cv::Mat img_;
    cv::Mat blurred_;
    cv::Mat edges_;

    std::string window_name{ "Edges" };

    int blur_amount{ 0 };
    const int max_blur_amount{ 20 };

    const std::array<int, 3> aperture_size{ 3, 5, 7 };
    int aperture_index{ 0 };
    const int max_aperture_index{ 2 };

    int low_threshold{ 50 };
    int high_threshold{ 100 };
    const int max_threshold{ 1000 };


    void convertToGray() {
        if (img_.channels() > 1) {
            cv::cvtColor(img_, img_, cv::COLOR_BGR2GRAY);
        }
    }
};

void applyCanny(int, void* data) {
    auto* ed{ static_cast<EdgeDetection*>(data) };

    if (ed->getBlurAmount()) {
        cv::GaussianBlur(ed->getImg(),
            ed->getBlurred(),
            cv::Size(2 * ed->getBlurAmount() + 1, 2 * ed->getBlurAmount() + 1),
            0);
    }
    else {
        ed->getBlurred() = ed->getImg().clone();
    }

    int aperture_size = ed->getApertureSize()[ed->getApertureIndex()];
    cv::Canny(ed->getBlurred(),
        ed->getEdges(),
        ed->getLowThreshold(),
        ed->getHighThreshold(),
        aperture_size);

    cv::imshow(ed->getWindowName(), ed->getEdges());

}

int main() {
    // Path to an image 
    std::string path{ "../data/images/sample.jpg" };

    // Load the image
    cv::Mat img{ cv::imread(path) };

    // Check if the image is loaded correctly
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    EdgeDetection ed{ img };
    cv::namedWindow(ed.getWindowName(), cv::WINDOW_AUTOSIZE);

    // Callback for threshold
    cv::createTrackbar("Low Threshold", ed.getWindowName(), &ed.getLowThreshold(), ed.getMaxThreshold(), applyCanny, &ed);
    cv::createTrackbar("High Threshold", ed.getWindowName(), &ed.getHighThreshold(), ed.getMaxThreshold(), applyCanny, &ed);

    // Callback for aperture
    cv::createTrackbar("Aperture", ed.getWindowName(), &ed.getApertureIndex(), ed.getMaxApertureIndex(), applyCanny, &ed);

    // Callback for blur
    cv::createTrackbar("Blur", ed.getWindowName(), &ed.getBlurAmount(), ed.getMaxBlurAmount(), applyCanny, &ed);

    cv::imshow("Original", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
