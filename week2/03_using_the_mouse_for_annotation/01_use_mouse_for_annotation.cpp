#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <cmath>

struct UserData {
    cv::Mat img{};
    cv::Mat clone{};
    cv::Point center{};
    cv::Point end{};
    const std::string winname{ "Image" };
};

void drawCircle(int action, int x, int y, int flags, void* data) {
    UserData* ud = static_cast<UserData*>(data);

    if (action == cv::EVENT_LBUTTONDOWN) {
        ud->center = cv::Point(x, y);
        cv::circle(ud->img, ud->center, 1, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
        std::cout << ud->center << '\n';
    }
    else if (action == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON)) {
        ud->clone = ud->img.clone();
        float radius = std::hypot(ud->center.x - ud->end.x, ud->center.y - ud->end.y);
        cv::circle(ud->clone, ud->center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
        cv::imshow(ud->winname, ud->clone);
    }
    else if (action == cv::EVENT_LBUTTONUP) {
        ud->end = cv::Point(x, y);
        float radius = std::hypot(ud->center.x - ud->end.x, ud->center.y - ud->end.y);
        cv::circle(ud->img, ud->center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
        cv::imshow(ud->winname, ud->img);
    }
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/sample.jpg" };
    // Load the image
    UserData ud;
    ud.img = cv::imread(path, cv::IMREAD_COLOR);
    ud.clone = ud.img.clone();

    try {
        if (ud.img.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    cv::namedWindow(ud.winname, cv::WINDOW_NORMAL);

    cv::setMouseCallback(ud.winname, drawCircle, &ud);

    while (true) {
        cv::putText(ud.img,
            "Choose center, and drag, Press q to exit and c to clear",
            cv::Point(10, 30),
            cv::FONT_HERSHEY_TRIPLEX,
            0.7,
            cv::Scalar(255, 255, 255));

        cv::imshow(ud.winname, ud.img);
        char k = cv::waitKey(0);

        if (k == 'q') {
            break;
        }

        if (k == 'c') {
            ud.img = cv::imread(path, cv::IMREAD_COLOR);
        }

    }
    cv::destroyAllWindows();
}
