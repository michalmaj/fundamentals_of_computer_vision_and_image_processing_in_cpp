#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <format>
#include <vector>
#include <numbers>
#include <ranges>
#include <array>

int main() {

    // 1. Load images and set exposures
    // Load images from given path
    constexpr std::array<std::string, 4> file_names{ "img_0.033.jpg", "img_0.25.jpg", "img_2.5.jpg", "img_15.jpg" };
    std::vector<cv::Mat> images;
    images.reserve(4);
    std::ranges::for_each(file_names, [&images](const auto& name) {
        std::string path{ std::format("../data/images/{}", name) };
        images.emplace_back(cv::imread(path));
        if (images.back().empty()) {
            std::cerr << std::format("Can't load file from path: {}", path) << '\n';
            images.pop_back();
        }
        });

    // Load times
    constexpr std::array<float, 4> times{ 1 / 30.0f, 0.25, 2.5, 15.0 };

    // 2. Alignment set of images
    cv::Ptr<cv::AlignMTB> align_mtb{ cv::createAlignMTB() };
    align_mtb->process(images, images);

    // 3. Recover the Camera Response Function (CRF)
    cv::Mat response_debevec, response_robertson;

    // Calibrate Debevec
    cv::Ptr<cv::CalibrateDebevec> calibrate_debevec{ cv::createCalibrateDebevec() };
    calibrate_debevec->process(images, response_debevec, times);

    // Calibrate Robertson
    cv::Ptr<cv::CalibrateRobertson> calibrate_robertson{ cv::createCalibrateRobertson() };
    calibrate_robertson->process(images, response_robertson, times);

    // 4. Merge images
    cv::Mat hdr_debevec, hdr_robertson;

    // Merge Debevec
    cv::Ptr<cv::MergeDebevec> merge_debevec{ cv::createMergeDebevec() };
    merge_debevec->process(images, hdr_debevec, times, response_debevec);

    // Merge Robertson
    cv::Ptr<cv::MergeRobertson> merge_robertson{ cv::createMergeRobertson() };
    merge_robertson->process(images, hdr_robertson, times, response_robertson);

    // 5. Tone mapping - three methods
    // Drago
    cv::Mat drago_debevec, drago_robertson;
    cv::Ptr<cv::TonemapDrago> tonemap_drago{ cv::createTonemapDrago(1.0, 0.7) };
    
    tonemap_drago->process(hdr_debevec, drago_debevec);
    drago_debevec = 3 * drago_debevec;

    tonemap_drago->process(hdr_robertson, drago_robertson);
    drago_robertson = 3 * drago_robertson;

    // Reinhard
    cv::Mat reinhard_debevec, reinhard_robertson;
    cv::Ptr<cv::TonemapReinhard> tonemap_reinhard{ cv::createTonemapReinhard(1.5, 0, 0, 0) };

    tonemap_reinhard->process(hdr_debevec, reinhard_debevec);

    tonemap_reinhard->process(hdr_robertson, reinhard_robertson);

    // Mantiuk
    cv::Mat mantiuk_debevec, mantiuk_robertson;
    cv::Ptr<cv::TonemapMantiuk> tonemap_mantiuk{ cv::createTonemapMantiuk(2.2, 0.85, 1.2) };

    tonemap_mantiuk->process(hdr_debevec, mantiuk_debevec);
    mantiuk_debevec = 3 * mantiuk_debevec;

    tonemap_mantiuk->process(hdr_robertson, mantiuk_robertson);
    mantiuk_robertson = 3 * mantiuk_robertson;

    // Create view for names and images of different exposures
    if (file_names.size() != images.size()) {
        throw std::runtime_error("Different size of loaded images and corresponding names");
    }
    auto view = std::views::zip(file_names, images);

    for (auto&& [name, image] : view) {
        cv::namedWindow(name, cv::WINDOW_NORMAL);
        cv::imshow(name, image);
    }

    cv::namedWindow("Drago Debevec", cv::WINDOW_NORMAL);
    cv::namedWindow("Drago Robertson", cv::WINDOW_NORMAL);
    cv::namedWindow("Reinhard Debevec", cv::WINDOW_NORMAL);
    cv::namedWindow("Reinhard Robertson", cv::WINDOW_NORMAL);
    cv::namedWindow("Mantiuk Debevec", cv::WINDOW_NORMAL);
    cv::namedWindow("Mantiuk Robertson", cv::WINDOW_NORMAL);
    cv::imshow("Drago Debevec", drago_debevec);
    cv::imshow("Drago Robertson", drago_robertson);
    cv::imshow("Reinhard Debevec", reinhard_debevec);
    cv::imshow("Reinhard Robertson", reinhard_robertson);
    cv::imshow("Mantiuk Debevec", mantiuk_debevec);
    cv::imshow("Mantiuk Robertson", mantiuk_robertson);
    cv::waitKey(0);
    cv::destroyAllWindows();


    return 0;
}
