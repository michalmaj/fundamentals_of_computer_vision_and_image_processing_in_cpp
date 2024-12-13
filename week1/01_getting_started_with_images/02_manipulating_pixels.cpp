#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <print>

int main() {
    // Create a name for the original OpenCV window
    std::string windowName{ "Original Image" };
    // Allow resizing the window by mouse (WINDOW_NORMAL flag is necessary)
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);

    // Create a name for the modified OpenCV window
    std::string windowNameMod{ "Modified Image" };
    // Allow resizing the window by mouse (WINDOW_NORMAL flag is necessary)
    cv::namedWindow(windowNameMod, cv::WINDOW_NORMAL);

    // Load an image from the given path. IMREAD_GRAYSCALE loads the image as a single-channel grayscale image.
    cv::Mat img{ cv::imread("../data/images/number_zero.jpg", cv::IMREAD_GRAYSCALE) };

    // Check if the loaded image is valid
    if (img.empty()) {
        std::cerr << "Error: Cannot open the file ../data/images/number_zero.jpg\n";
        return EXIT_FAILURE;
    }

    // Display the original matrix values
    std::cout << "Matrix before modification:\n" << img << "\n\n";
    cv::imshow(windowName, img);

    // Modify pixel value at row: 1, column: 0
    img.at<uchar>(1, 0) = 200;

    // Display the modified matrix values
    std::cout << "Matrix after modification:\n" << img << "\n\n";
    cv::imshow(windowNameMod, img);

    // Wait indefinitely until a key is pressed, then clean up
    cv::waitKey(0);
    cv::destroyAllWindows();
}
