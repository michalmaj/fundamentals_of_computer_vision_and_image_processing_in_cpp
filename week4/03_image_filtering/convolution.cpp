#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <vector>



int main() {
    // Path to an image 
    std::string path{ "../data/images/girl.jpg" };

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

    // Set kernel size
    int kernel_size{ 5 };

    // Create a 5x5 kernel with all elements equal to 1
    cv::Mat kernel{ cv::Mat::ones(kernel_size, kernel_size, CV_32F) };

    // Normalize kernel so sum of all elements equals 1
    kernel = kernel / static_cast<float>(kernel_size * kernel_size);

    // Print kernel
    std::cout << kernel;
    std::cout << std::endl;

    cv::Mat output;

    // Apply filter
    cv::filter2D(img, output, -1, kernel);

    cv::imshow("Original", img);
    cv::imshow("After filtration", output);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
