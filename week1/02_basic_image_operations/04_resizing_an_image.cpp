#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

// Resize by width or height
cv::Mat resize(const cv::Mat& mat, int value, const std::string& method = "width", int interpolation = cv::INTER_NEAREST) {
    // Throw an error if mat object is empty
    if (mat.empty()) {
        throw std::runtime_error("Empty image!\n");
    }

    // Throw an error if value is lower than 1 
    if (value <= 0) {
        throw std::invalid_argument(std::format("Your {} must be greater than 0!\n", method));
    }

    // Get width and height of current image
    auto [w, h] = mat.size();
    double ratio = w / static_cast<double>(h);

    // Set new value for height and width
    int new_width = value;
    int new_height = value;

    // Calculate new width or height by given method
    if (method == "height") {
        // To calculate new width one must multiply new height by calculated ratio
        new_width = static_cast<int>(std::round(new_height * ratio));
    }
    else if (method == "width") {
        // To calculate new height one must divide new width by given ration
        new_height = static_cast<int>(std::round(new_width / ratio));
    }
    else {
        throw std::runtime_error("Unknown parameter\n");
    }

    cv::Mat resized;
    cv::resize(mat, resized, cv::Size(new_width, new_height), 0, 0, interpolation);

    return resized;
}

cv::Mat resizeByHeight(const cv::Mat& mat, int new_height, int interpolation = cv::INTER_NEAREST) {
    auto resized = resize(mat, new_height, "height", interpolation);
    return resized;
}

cv::Mat resizeByWidth(const cv::Mat& mat, int new_width, int interpolation = cv::INTER_NEAREST) {
    auto resized = resize(mat, new_width, "width", interpolation);
    return resized;
}

int main() {
    // Path to image
    std::string path{ "../data/images/boy.jpg" };

    // Create windows
    std::string original{ "Original Image" };
    cv::namedWindow(original, cv::WINDOW_NORMAL);

    std::string down_size{ "Down Size" };
    cv::namedWindow(down_size, cv::WINDOW_NORMAL);

    std::string inter_nearest{ "Inter Nearest" };
    cv::namedWindow(inter_nearest, cv::WINDOW_NORMAL);

    std::string inter_linear{ "Inter Linear" };
    cv::namedWindow(inter_linear, cv::WINDOW_NORMAL);

    std::string inter_cubic{ "Inter Cubic" };
    cv::namedWindow(inter_cubic, cv::WINDOW_NORMAL);

    std::string inter_area{ "Inter Area" };
    cv::namedWindow(inter_area, cv::WINDOW_NORMAL);

    std::string inter_lanczos{ "Inter Lanczos4" };
    cv::namedWindow(inter_lanczos, cv::WINDOW_NORMAL);

    std::string inter_area_exact{ "Inter Area Exact" };
    cv::namedWindow(inter_area_exact, cv::WINDOW_NORMAL);


    // Load an image from the given path. IMREAD_COLOR loads the image as a BGR image.
    cv::Mat img{ cv::imread(path, cv::IMREAD_COLOR) };

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

    // Resize Images
    // 1. Scale down by a 30% in width and 50% in height
    cv::Mat scale_down;
    cv::resize(img, scale_down, cv::Size(), 0.7, 0.5);

    // 2. Different methods to scale up 
    auto nearest = resizeByHeight(img, 768);
    auto linear = resizeByWidth(img, 1024, cv::INTER_LINEAR);
    auto cubic = resizeByHeight(img, 796, cv::INTER_CUBIC);
    auto area = resizeByWidth(img, 1024, cv::INTER_AREA);
    auto lanczos = resizeByHeight(img, 768, cv::INTER_LANCZOS4);
    auto exact = resizeByWidth(img, 1024, cv::INTER_LINEAR_EXACT);


    cv::imshow(original, img);
    cv::imshow(down_size, scale_down);
    cv::imshow(inter_nearest, nearest);
    cv::imshow(inter_linear, linear);
    cv::imshow(inter_cubic, cubic);
    cv::imshow(inter_area, area);
    cv::imshow(inter_lanczos, lanczos);
    cv::imshow(inter_area_exact, exact);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
