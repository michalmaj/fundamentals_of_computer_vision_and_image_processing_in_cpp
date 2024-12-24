#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

struct UserData {
    cv::Mat img;
    std::string windowName{ "Resize Image" };
    std::string trackbarValue{ "Scale" };
    std::string trackbarType{ "Type: \n 0: Scale Up \n 1: Scale Down" };

    int maxScaleUp = 100;
    int scaleFactor = 1;
    int scaleType = 0;
    int maxType = 1;

};

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

    cv::namedWindow(ud.windowName, cv::WINDOW_NORMAL);

}
