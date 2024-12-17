#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

void checkImage(const cv::Mat& mat) {
    if (mat.empty()) {
        throw std::runtime_error("Empty image!\n");
    }
}

void checkPoints(const cv::Mat& mat, const cv::Point& p) {
    if (p.x < 0 || p.x >= mat.size().width || p.y < 0 || p.y >= mat.size().height) {
        throw std::range_error("Point is not part of the image!\n");
    }
}

cv::Mat drawText(const cv::Mat& mat,
    const std::string& text,
    cv::Point started_point,
    int pixel_height,
    int font_face,
    const cv::Scalar& font_color,
    int thickness = 1,
    int line_type = cv::LINE_AA,
    int baseline = 0) {

    // Check if image is empty
    checkImage(mat);
    // Check if points are inside image
    checkPoints(mat, started_point);

    // Get font scale 
    auto font_scale = cv::getFontScaleFromHeight(font_face, pixel_height, thickness);

    // Get text size (width and height)
    auto size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    // If point are out of band with text size, clamp them
    started_point.x = std::clamp(started_point.x, 0, mat.cols - size.width);
    started_point.y = std::clamp(started_point.y, size.height, mat.rows - baseline);

    // Create new image with text
    cv::Mat img = mat.clone();
    cv::putText(img, text, started_point, font_face, font_scale, font_color, thickness, line_type);

    return img;
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/boy.jpg" };
    // Load the image
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    cv::Mat img_text = drawText(img, "Hello", cv::Point(0, 0), 20, cv::FONT_HERSHEY_TRIPLEX, cv::Scalar(0, 0, 0), 3);

    cv::imshow("Original image", img);
    cv::imshow("Image with text", img_text);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
