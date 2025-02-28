#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

struct UserData {
    cv::Mat img;
    cv::Mat scaled_img;
    std::string window_name{ "Resize Image" };
    std::string trackbar_value{ "Scale" };
    std::string trackbar_type{ "Type: \n 0: Scale Up \n 1: Scale Down" };

    int max_scale_up = 100;
    int scale_factor = 1;
    int scale_type = 0;
    int max_type = 1;
};

void scaleImg(int, void* data) {
    UserData* ud = static_cast<UserData*>(data);

    double scale_factor = 1 + (ud->scale_type == 0 ? ud->scale_factor / 100.0 : -ud->scale_factor / 100.0);

    if (scale_factor <= 0) {
        scale_factor = 0.1; 
    }

    cv::resize(ud->img, ud->scaled_img, cv::Size(), scale_factor, scale_factor, cv::INTER_LANCZOS4);
    cv::imshow(ud->window_name, ud->scaled_img);
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/truth.png" };

    // Load the image
    UserData ud;
    ud.img = cv::imread(path);

    try {
        if (ud.img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    cv::namedWindow(ud.window_name, cv::WINDOW_AUTOSIZE);

    // Create trackbars
    cv::createTrackbar(ud.trackbar_value, ud.window_name, &ud.scale_factor, ud.max_scale_up, scaleImg, &ud);
    cv::createTrackbar(ud.trackbar_type, ud.window_name, &ud.scale_type, ud.max_type, scaleImg, &ud);

    // Initial display
    scaleImg(0, &ud);

    while (true) {
        auto key = cv::waitKey(20);
        if (key == 'q') {
            break;
        }
    }

    cv::destroyWindow(ud.window_name);
    return 0;
}