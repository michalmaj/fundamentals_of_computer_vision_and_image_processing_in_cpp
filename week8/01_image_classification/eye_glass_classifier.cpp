#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <filesystem>
#include <print>
#include <ranges>
#include <unordered_set>
#include <random>
#include <span>


class TestTrainData {
public:
    TestTrainData(const std::filesystem::path& path, size_t number_of_classes = 2, float test_size = 0.2f)
        :
        number_of_classes_(number_of_classes),
        test_size_(test_size) {
        if (!std::filesystem::exists(path) or !std::filesystem::is_directory(path)) {
            std::cerr << std::format("Wrong path: {}", path.string());
            return;
        }
        size_t nums{ 0 };
        for (const auto& dirs : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_directory(dirs)) {
                for (const auto& file : std::filesystem::directory_iterator(dirs)) {
                    if (std::filesystem::is_regular_file(file) and valid_extensions_.contains(std::ranges::to<std::string>(file.path().extension().string() | std::views::transform(::tolower)))) {
                        cv::Mat img{ cv::imread(file.path().string()) };
                        if (img.empty()) {
                            std::cerr << std::format("Can't load file: {}\n", file.path().string());
                            continue;
                        }
                        data_.emplace_back(std::move(img));
                    }
                }
                prepare_data(nums);
                nums++;
            }
        }

        if (nums != number_of_classes_) {
            throw std::runtime_error("Numer of classes mismatch!\n");
        }
    }

    auto getTrainImages() const { return train_; }
    auto getTestImages() const { return test_; }
    auto getTrainLabels() const { return train_labels_; }
    auto getTestLabels() const { return test_labels_; }

private:
    std::vector<cv::Mat> data_;
    std::vector<cv::Mat> train_;
    std::vector<cv::Mat> test_;
    std::vector<size_t> train_labels_;
    std::vector<size_t> test_labels_;

    size_t number_of_classes_{};
    float test_size_{};

    inline static std::unordered_set<std::string> valid_extensions_{ ".jpg", ".jpeg", ".png" };

    void prepare_data(size_t nums) {
        shuffle_data(data_);
        auto size{ data_.size() };
        size_t take{ static_cast<size_t>(size * test_size_) };
        test_.append_range(std::views::reverse(data_) | std::views::take(take));
        test_labels_.append_range(std::views::repeat(nums) | std::views::take(take));
        train_.append_range(data_ | std::views::take(size - take));
        train_labels_.append_range(std::views::repeat(nums) | std::views::take(size - take));
        data_.clear();
    }

    void shuffle_data(std::vector<cv::Mat>& mat) {
        std::random_device rd;
        std::mt19937 seed(rd());
        std::ranges::shuffle(mat, seed);
    }
};

class HogFeatureDescriptor {
public:
    HogFeatureDescriptor(std::span<const cv::Mat> images,
        cv::Size win_size = cv::Size(96, 32),
        cv::Size block_size = cv::Size(8, 8),
        cv::Size block_stride = cv::Size(8, 8),
        cv::Size cell_size = cv::Size(4, 4),
        int num_bins = 9,
        int deriv_aperture = 0,
        double win_sigma = 4.0,
        cv::HOGDescriptor::HistogramNormType histogram_norm_type = cv::HOGDescriptor::L2Hys,
        double l2_hys_thresh = 0.2,
        bool gamma_correction = true,
        int n_levels = 64,
        bool signed_gradient = true)
        :
        images_(images.begin(), images.end()),
        win_size_(win_size), block_size_(block_size), block_stride_(block_stride), cell_size_(cell_size),
        num_bins_(num_bins), deriv_aperture_(deriv_aperture), win_sigma_(win_sigma), histogram_norm_type_(histogram_norm_type),
        l2_hys_thresh_(l2_hys_thresh), gamma_correction_(gamma_correction), n_levels_(n_levels), signed_gradient_(signed_gradient) {

        // Create hog
        hog_ = cv::HOGDescriptor{ win_size_, block_size_, block_stride_, cell_size_, num_bins_,
          deriv_aperture_, win_sigma_, histogram_norm_type_, l2_hys_thresh_, gamma_correction_, n_levels_, signed_gradient_ };

        calculateDescriptors();
    }

    cv::Mat getSamples() {
        cv::Mat mat(hog_descriptors_.size(), hog_descriptors_[0].size(), CV_32FC1);
        std::ranges::for_each(hog_descriptors_ | std::views::enumerate, [&](auto&& index_row) {
            auto [i, row] = index_row;
            std::ranges::copy(row, mat.ptr<float>(i));
            });
        return mat;
    }

private:
    std::vector<cv::Mat> images_;
    // Setup hog features
    cv::Size win_size_;
    cv::Size block_size_;
    cv::Size block_stride_;
    cv::Size cell_size_;
    int num_bins_{};
    int deriv_aperture_{};
    double win_sigma_{};
    cv::HOGDescriptor::HistogramNormType histogram_norm_type_;
    double l2_hys_thresh_{};
    bool gamma_correction_{};
    int n_levels_{};
    bool signed_gradient_{};

    // Hog setup and hog descriptors
    cv::HOGDescriptor hog_;
    std::vector<std::vector<float>> hog_descriptors_;

    void calculateDescriptors() {
        hog_descriptors_.reserve(images_.size());
        hog_descriptors_ = images_ |
            std::views::transform([this](const auto& img) {
            std::vector<float> descriptors;
            hog_.compute(img, descriptors);
            return descriptors;
                }) |
            std::ranges::to<std::vector>();
    }
};

int main() {
    std::filesystem::path path{ "../data/images/glassesDataset" };

    TestTrainData t{ path };

    //for (const auto& dirs : std::filesystem::directory_iterator(path)) {
    //    if (std::filesystem::is_directory(dirs)) {
    //        for (const auto& e : std::filesystem::directory_iterator(dirs)) {
    //            std::println("{}", e.path().string());
    //        }
    //    }
    //}

    HogFeatureDescriptor hog{ t.getTrainImages() };

    std::vector<std::string> names{ ".JPG", ".PNG" };
    for (const auto& e : names) {
        auto view = e | std::views::transform(::tolower) | std::ranges::to<std::string>();
        std::cout << view << '\n';
    }


    return 0;
}
