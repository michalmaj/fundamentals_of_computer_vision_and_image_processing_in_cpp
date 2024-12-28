#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/dilation_example.jpg" };

    // Load the image
    cv::Mat dilation{ cv::imread(path) };

    path = "../data/images/erosion_example.jpg";
    cv::Mat erosion{ cv::imread(path) };

    // Check if the image loaded correctly
    try {
        if (dilation.empty() or erosion.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    int kernel_size{ 7 };
    auto big_kernel{ cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernel_size, kernel_size)) };
    cv::namedWindow("Big Kernel", cv::WINDOW_NORMAL);
    kernel_size = 3;
    auto small_kernel{ cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernel_size, kernel_size)) };
    cv::namedWindow("Small Kernel", cv::WINDOW_NORMAL);

    // Dilatation version
    cv::Mat dilatation_big_kernel;
    cv::dilate(dilation, dilatation_big_kernel, big_kernel);

    cv::Mat dilation_small_kernel_one_iteration;
    cv::Mat dilation_small_kernel_two_iterations;
    cv::dilate(dilation, dilation_small_kernel_one_iteration, small_kernel, cv::Point(-1, -1), 1);
    cv::dilate(dilation, dilation_small_kernel_two_iterations, small_kernel, cv::Point(-1, -1), 2);

    // Erosion version
    cv::Mat erosion_big_kernel;
    cv::erode(erosion, erosion_big_kernel, big_kernel);

    cv::Mat erosion_small_kernel_one_iteration;
    cv::Mat erosion_small_kernel_two_iterations;
    cv::erode(erosion, erosion_small_kernel_one_iteration, small_kernel, cv::Point(-1, -1), 1);
    cv::erode(erosion, erosion_small_kernel_two_iterations, small_kernel, cv::Point(-1, -1), 2);

    cv::imshow("Dilation", dilation);
    cv::imshow("Erosion", erosion);
    cv::imshow("Big Kernel", big_kernel * 255);
    cv::imshow("Small Kernel", small_kernel * 255);
    cv::imshow("Dilation Big Kernel", dilatation_big_kernel);
    cv::imshow("Dilation Small Kernel One Iteration", dilation_small_kernel_one_iteration);
    cv::imshow("Dilation Small Kernel Two Iterations", dilation_small_kernel_two_iterations);
    cv::imshow("Erosion Big Kernel", erosion_big_kernel);
    cv::imshow("Erosion Small Kernel One Iteration", erosion_small_kernel_one_iteration);
    cv::imshow("Erosion Small Kernel Two Iterations", erosion_small_kernel_two_iterations);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
