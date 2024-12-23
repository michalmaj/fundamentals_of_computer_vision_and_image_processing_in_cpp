/*
 * Create a Face Annotation Tool
 * Now that you have learnt how to use mouse for annotation with OpenCV, it's time to put your concepts to a test.
 * In this assignment, you will create a GUI application which will let the user create a bounding box around a face present in
 * the input image and save the cropped face automatically. *
 */

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

struct UserData {
    cv::Mat img{};
    cv::Mat temp{};
    std::optional<cv::Point> pt1{ std::nullopt };
    std::optional<cv::Point> pt2{ std::nullopt };
    const std::string winname{ "Image" };
};

void drawRectangle(int action, int x, int y, int flags, void* data) {
    UserData* ud = static_cast<UserData*>(data);

    if (action == cv::EVENT_LBUTTONDOWN && !ud->pt1.has_value()) {
        ud->pt1 = cv::Point(x, y);
        cv::circle(ud->img, ud->pt1.value(), 1, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
        cv::imshow(ud->winname, ud->img);
    }
    else if (action == cv::EVENT_LBUTTONDOWN && !ud->pt2.has_value()) {
        ud->pt2 = cv::Point(x, y);
        cv::circle(ud->img, ud->pt2.value(), 1, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
        cv::imshow(ud->winname, ud->img);
    }

    if (ud->pt1.has_value() && ud->pt2.has_value()) {
        cv::rectangle(ud->img, ud->pt1.value(), ud->pt2.value(), cv::Scalar(255, 0, 255), 2, cv::LINE_AA);
        cv::imshow(ud->winname, ud->img);
    }
}

int main() {
    // Path to an image 
    std::string path{ "../data/images/sample.jpg" };
    // Load the image
    UserData ud;
    ud.img = cv::imread(path, cv::IMREAD_COLOR);
    ud.temp = ud.img.clone();
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
    cv::setMouseCallback(ud.winname, drawRectangle, &ud);

    while (true) {
        cv::imshow(ud.winname, ud.img);
        char key = cv::waitKey(0);

        if (key == 'q') {
            break;
        }

        if (key == 's') {
            if (ud.pt1.has_value() && ud.pt2.has_value()) {
                cv::Mat roi = ud.img(cv::Range(ud.pt1.value().y, ud.pt2.value().y),
                    cv::Range(ud.pt1.value().x, ud.pt2.value().x));
                cv::imwrite("../data/images/roi.jpg", roi);
            }
        }

      if (key == 'r') {
          ud.pt1 = std::nullopt;
          ud.pt2 = std::nullopt;
          ud.img = ud.temp.clone();
      }
    }

        cv::destroyAllWindows();
    }
