#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <print>

int main() {

    // Path to a video
    std::string path{ "../data/videos/chaplin.mp4" };

    // Create object of video capture class
    cv::VideoCapture cap(path);

    // Chek if stream is open
    try {
        if (!cap.isOpened()) {
            throw std::runtime_error(std::format("Can't open video from given path: {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }

    // Get information about video
    double fps = cap.get(cv::CAP_PROP_FPS);
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::println("Video properties: width, height, fps: {}x{}@{}fps", width, height, fps);

    // Set delay
    auto delay = static_cast<int>(1000 / cap.get(cv::CAP_PROP_FPS));

    // Create a new window with given name
    std::string window_name{ "Frame" };
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);

    // Create object for writing video
    std::string write_path{ "../data/videos/chaplin_10fps.mp4" };
    auto codec{ cv::VideoWriter::fourcc('M', 'J', 'P', 'G') };
    double write_fps{ 10 };
    cv::Size write_size{ width, height };

    cv::VideoWriter out(write_path, codec, write_fps, write_size);

    // Main loop (until there is at least 1 frame in loaded video)
    while (cap.isOpened()) {
        // Create space for frames from video
        cv::Mat frame;
        cap.read(frame);

        // If there is no frame break from main loop
        if (frame.empty()) {
            break;
        }

        // Show next frame
        cv::imshow(window_name, frame);

        // Write frame into the file
        out.write(frame);

        // Get a keystroke and set delay
        auto key = cv::waitKey(delay);

        // If q is pressed break from the loop
        if (key == 'q') {
            break;
        }
        else if (key == 'p') { // pause the video
            cv::waitKey(0);
        }
        else if (key == 'c') { // unpause the video
            cv::waitKey(delay);
        }
    }

    // Release stream
    cap.release();
    out.release(); // Release video writer object
    cv::destroyWindow(window_name);
}
