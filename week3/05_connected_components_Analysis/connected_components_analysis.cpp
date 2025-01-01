#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>

int main() {
    // Path to an image 
    std::string path{ "../data/images/truth.png" };

    // Load an image
    auto grayscale{ cv::imread(path, cv::IMREAD_GRAYSCALE) };

    // Check if an image exists
    try {
        if (grayscale.empty()) {
            throw std::runtime_error(std::format("Can't load an image from {}\n", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }

    // Use threshold on the image
    cv::Mat threshold;
    cv::threshold(grayscale, threshold, 127, 255, cv::THRESH_BINARY);

    // Find connected components
    cv::Mat labels;
    auto n_components{ cv::connectedComponents(threshold, labels) };

    // Make a copy of connected components, because it is possible to show every label separately
    auto labels_copy = labels.clone();

    // Normalize labels to [0, 255] range
    cv::normalize(labels, labels, 0, 255, cv::NORM_MINMAX);

    // Image conversion to unsigned 8-bits with 1 channel
    labels.convertTo(labels, CV_8U);

    // Add a colormap for better visual experience
    cv::Mat color_labels;
    cv::applyColorMap(labels, color_labels, cv::COLORMAP_JET);

    cv::imshow("grayscale", grayscale);
    cv::imshow("connected components", labels);
    for (int i{ 0 }; i < n_components; ++i) {
        std::string name{ std::format("Label {}", i + 1) };
        cv::imshow(name, labels_copy == i);
    }
    cv::imshow("Colormap", color_labels);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
