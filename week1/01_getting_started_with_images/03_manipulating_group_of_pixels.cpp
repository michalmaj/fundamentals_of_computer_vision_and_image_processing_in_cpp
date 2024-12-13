#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

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
    try {
        if (img.empty()) {
            throw std::runtime_error("Error: Cannot open the file ../data/images/number_zero.jpg\n");
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    // Display the original matrix values
    std::cout << "Matrix before modification:\n" << img << "\n\n";
    cv::imshow(windowName, img);

    // Modify group of pixels at given range [Rows(start, end), Columns(start, end)]
    // In this case we want 2 last rows and 4 last columns
    try {
        if (img.rows < 2 || img.cols < 4) {
            throw std::out_of_range("Image is too small to perform ROI operation.\n");
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
    auto rows = cv::Range(img.rows - 2, img.rows); // Set rows
    auto columns = cv::Range(img.cols - 4, img.cols); // Set columns
    cv::Mat roi = img(rows, columns); // Getting a roi
    std::cout << "Selected region of interest:\n" << roi << "\n\n";

    // Set ROI to specyfic value, it is possible to set new value directly to img or to roi (it is an alias, not a copy)
    roi.setTo(222);

    // Display the modified matrix values
    std::cout << "Matrix after modification:\n" << img << "\n\n";
    cv::imshow(windowNameMod, img);

    // Wait indefinitely until a key is pressed, then clean up
    cv::waitKey(0);
    cv::destroyAllWindows();
}
