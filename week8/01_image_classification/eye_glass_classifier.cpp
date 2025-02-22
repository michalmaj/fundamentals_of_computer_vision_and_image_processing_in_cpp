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
      static std::random_device rd;
      static std::mt19937 seed(rd());
      std::ranges::shuffle(mat, seed);
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

    return 0;
}
