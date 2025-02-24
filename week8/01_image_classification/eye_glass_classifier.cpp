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
#include <memory>


class TestTrainData {
public:
    /// <summary>
    /// Loading given datasets
    /// </summary>
    /// <param name="path">Path to the directory containing class subfolders with images.</param>
    /// <param name="number_of_classes">Number of expected classes (subfolders).</param>
    /// <param name="test_size">Proportion of the dataset to be used for testing (e.g., 0.2 means 20% test, 80% train).</param>
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
    std::vector<int> train_labels_;
    std::vector<int> test_labels_;

    size_t number_of_classes_{};
    float test_size_{};

    inline static std::unordered_set<std::string> valid_extensions_{ ".jpg", ".jpeg", ".png" };

    void prepare_data(size_t nums) {
        shuffle_data(data_);
        auto size{ data_.size() };
        int take{ static_cast<int>(size * test_size_) };
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
    /// <summary>
    /// HOG setup for detection problem. Its main purpose is feature extraction form set of images.
    /// </summary>
    /// <param name="win_size">The size of the detection window (e.g., 96x32 for signs, 128x64 for pedestrians).</param>
    /// <param name="block_size">The size of a single HOG block (e.g., 8x8). A block contains multiple cells.</param>
    /// <param name="block_stride">The step size for moving the block (e.g., 8x8, meaning blocks do not overlap).</param>
    /// <param name="cell_size">The size of an individual cell (e.g., 4x4). Each block consists of multiple cells.</param>
    /// <param name="num_bins">The number of histogram bins (e.g., 9 for a range of 0-180° with 20° steps).</param>
    /// <param name="deriv_aperture">The aperture size for the Sobel operator (default is 1; if 0, OpenCV sets it automatically).</param>
    /// <param name="win_sigma">The standard deviation for the Gaussian smoothing filter used in normalization (default: 4.0).</param>
    /// <param name="histogram_norm_type">The histogram normalization method (L2Hys is commonly used).</param>
    /// <param name="l2_hys_thresh">The L2-Hysteresis normalization threshold (default: 0.2; lower values improve stability).</param>
    /// <param name="gamma_correction">Whether to apply gamma correction (true improves robustness to lighting variations).</param>
    /// <param name="n_levels">The number of scale levels in the image pyramid (higher values improve multi-scale detection).</param>
    /// <param name="signed_gradient">Whether the gradients are signed (true: range -180° to 180°, false: range 0°-180°).</param>
    HogFeatureDescriptor(
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
        win_size_(win_size), block_size_(block_size), block_stride_(block_stride), cell_size_(cell_size),
        num_bins_(num_bins), deriv_aperture_(deriv_aperture), win_sigma_(win_sigma), histogram_norm_type_(histogram_norm_type),
        l2_hys_thresh_(l2_hys_thresh), gamma_correction_(gamma_correction), n_levels_(n_levels), signed_gradient_(signed_gradient) {

        // Create hog
        hog_ = cv::HOGDescriptor{ win_size_, block_size_, block_stride_, cell_size_, num_bins_,
          deriv_aperture_, win_sigma_, histogram_norm_type_, l2_hys_thresh_, gamma_correction_, n_levels_, signed_gradient_ };
    }

    void loadImages(std::span<const cv::Mat> images) {
        if (images.empty()) {
            throw std::runtime_error("A collection of images is empty!\n");
        }
        images_ = { images.begin(), images.end() };
        calculateDescriptors();
    }

    cv::Mat getSamples() {
        if (images_.empty()) {
            std::cerr << "Load images first!\n";
            throw std::runtime_error("A collection of images is empty!\n");
        }
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

class SvmClassifier {
public:
    SvmClassifier(
        float regularization = 2.5f,
        float kernel_gamma = 0.02f,
        cv::ml::SVM::KernelTypes kernel_type = cv::ml::SVM::RBF,
        cv::ml::SVM::Types classificator_type = cv::ml::SVM::C_SVC)
        :
        regularization_(regularization),
        kernel_gamma_(kernel_gamma),
        kernel_type_(kernel_type),
        classificator_type_(classificator_type) {
        createModel();
    }

    void loadTrainTestData(const std::filesystem::path& path, size_t number_of_classes = 2, float test_size = 0.2f) {
        train_test_data_ = std::make_unique<TestTrainData>(path, number_of_classes, test_size);
    }

    void setHogFeatureDescriptor(
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
        bool signed_gradient = true) {
        if (!checkIfTrainTestExists()) {
            std::cerr << "First load train and test data!\n";
            throw std::runtime_error("There is no images to compute!\n");
        }

        train_descriptors_ = std::make_unique<HogFeatureDescriptor>(
            win_size,
            block_size,
            block_stride,
            cell_size,
            num_bins,
            deriv_aperture,
            win_sigma,
            histogram_norm_type,
            l2_hys_thresh,
            gamma_correction,
            n_levels,
            signed_gradient);
        train_descriptors_->loadImages(train_test_data_->getTrainImages());

        test_descriptions_ = std::make_unique<HogFeatureDescriptor>(
            win_size,
            block_size,
            block_stride,
            cell_size,
            num_bins,
            deriv_aperture,
            win_sigma,
            histogram_norm_type,
            l2_hys_thresh,
            gamma_correction,
            n_levels,
            signed_gradient);
        test_descriptions_->loadImages(train_test_data_->getTestImages());
    }

    void train(const std::filesystem::path& path = "../data/models/eyeGlassClassifierModel.yml") {
        cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(
            train_descriptors_->getSamples(),
            cv::ml::ROW_SAMPLE,
            train_test_data_->getTrainLabels());

        model->train(td);
        model->save(path.string());
    }

    void predict() {
        model->predict(test_descriptions_->getSamples(), responses_);
    }

    void evaluate() {
        float count{};
        float accuracy{};
        for (int i{ 0 }; i < responses_.rows; ++i) {
            if (responses_.at<  float>(i, 0) == train_test_data_->getTestLabels()[i]) {
                count += 1.0f;
            }
        }
        accuracy = (count / responses_.rows) * 100;
        std::cout << "Accuracy: " << accuracy << '\n';
    }

private:
    // Model
    cv::Ptr<cv::ml::SVM> model;
    // Model parameters
    float regularization_{};
    float kernel_gamma_{};
    cv::ml::SVM::KernelTypes kernel_type_{};
    cv::ml::SVM::Types classificator_type_{};

    // Model evaluate
    cv::Mat responses_;

    std::unique_ptr<TestTrainData> train_test_data_;
    std::unique_ptr<HogFeatureDescriptor> train_descriptors_;
    std::unique_ptr<HogFeatureDescriptor> test_descriptions_;

    // Model creation
    void createModel() {
        model = cv::ml::SVM::create();
        model->setGamma(kernel_gamma_);
        model->setC(regularization_);
        model->setKernel(kernel_type_);
        model->setType(classificator_type_);
    }

    bool checkIfTrainTestExists() const {
        return train_test_data_.get();
    }
};

int main() {
    std::filesystem::path path{ "../data/images/glassesDataset" };

    SvmClassifier svm;
    svm.loadTrainTestData(path);
    svm.setHogFeatureDescriptor();
    svm.train();
    svm.predict();
    svm.evaluate();

    return 0;
}
