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
#include <list>
#include <expected>

class ImageLoader {
public:
    void load_images(const std::filesystem::path& dir_path) {
        if (!std::filesystem::is_directory(dir_path)) {
            std::cerr << std::format("Wrong dir_path: {}", dir_path.string());
            throw std::runtime_error("Can't load data!\n");
        }

        images_.clear();

        for (const auto& file : std::filesystem::directory_iterator(dir_path)) {
            if (std::filesystem::is_regular_file(file) and valid_extensions_.contains(std::ranges::to<std::string>(file.path().extension().string() | std::views::transform(::tolower)))) {
                cv::Mat img{ cv::imread(file.path().string()) };
                if (img.empty()) {
                    std::cerr << std::format("Can't load file: {}\n", file.path().string());
                    continue;
                }
                images_.emplace_back(std::move(img));
            }
        }
    }

    [[nodiscard]] std::expected<std::vector<cv::Mat>, std::string> get_images() {
        if (images_.empty()) {
            return  std::unexpected("There is no images to process!\n");
        }

        return std::move(images_);
    }

private:
    std::vector<cv::Mat> images_;
    inline static std::unordered_set<std::string> valid_extensions_{ ".jpg", ".jpeg", ".png" };
};

class ModelLoader {
public:
    void load_model(const std::filesystem::path& model_path) {
        if (std::filesystem::is_regular_file(model_path) and valid_model_names_.contains(std::ranges::to<std::string>(model_path.extension().string() | std::views::transform(::tolower)))) {
            model_ = cv::ml::SVM::load(model_path.string());
        }
    }

    [[nodiscard]] std::expected<cv::Ptr<cv::ml::SVM>, std::string> get_model() {
        if (model_.empty()) {
            return  std::unexpected("Model is empty!\n");
        }

        return model_;
    }
private:
    cv::Ptr<cv::ml::SVM> model_;
    inline const static std::unordered_set<std::string> valid_model_names_{ ".yml" };
};

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
                image_loader_.load_images(dirs);
                auto expected = image_loader_.get_images();
                if (expected.has_value()) {
                    data_ = std::move(expected.value());
                }
                else {
                    std::cerr << expected.error() << std::endl;
                    throw std::runtime_error("Can't load data!\n");
                }
                prepare_data(nums);
                nums++;
            }
        }

        if (nums != number_of_classes_) {
            throw std::runtime_error("Numer of classes mismatch!\n");
        }
    }

    auto& getTrainImages() const { return train_; }
    auto& getTestImages() const { return test_; }
    auto& getTrainLabels() const { return train_labels_; }
    auto& getTestLabels() const { return test_labels_; }

private:
    ImageLoader image_loader_;
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

    void loadImages(const std::vector<cv::Mat>& images) {
        if (images.empty()) {
            throw std::runtime_error("A collection of images is empty!\n");
        }

        images_.clear();
        images_.reserve(images.size());
        std::ranges::transform(images, std::back_inserter(images_), [](const cv::Mat& img) {
            return img.clone();
            });

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

    [[nodiscard]] cv::HOGDescriptor getHog() const {
        return hog_;
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

class MultiScaleDetect {
public:
    auto getResults(
        const cv::Mat& img,
        cv::HOGDescriptor& hog,
        float hit_threshold = 1.0,
        cv::Size win_stride = cv::Size(8, 8),
        cv::Size padding = cv::Size(32, 32),
        float scale = 1.05,
        float final_threshold = 2,
        bool use_meanshift_grouping = false) {
        hog.detectMultiScale(img, bboxes_, weights_,
            hit_threshold, win_stride, padding, scale, final_threshold, use_meanshift_grouping);
        return std::make_pair(bboxes_, weights_);
    }

private:
    std::vector<cv::Rect> bboxes_;
    std::vector<double> weights_;
};

class ObjectDetector {
public:
    ObjectDetector(const std::filesystem::path& model_path,
        const std::filesystem::path& image_path,
        const cv::HOGDescriptor& hog_descriptor)
        :
        hog_descriptor_(hog_descriptor) {

        if (std::filesystem::is_regular_file(model_path)) {
            model_loader_.load_model(model_path);
            auto expected_model = model_loader_.get_model();
            if (expected_model.has_value()) {
                model_ = expected_model.value();
            }
            else {
                std::cerr << expected_model.error() << std::endl;
                throw std::runtime_error("Can't load model!\n");
            }
        }

        if (std::filesystem::is_directory(image_path)) {
            image_loader_.load_images(image_path);
            auto expected_images = image_loader_.get_images();
            if (expected_images.has_value()) {
                images_ = expected_images.value();
            }
            else {
                std::cerr << expected_images.error() << std::endl;
                throw std::runtime_error("Can't load images!\n");
            }
        }
    }

    void process(bool own_detector = true) {
        getSupportVectors();
        getDecisionFunction();
        getSvmTrainedDetector(own_detector);
        setSvmDetector();
    }

    void multiScaleDetection(
        const cv::Mat& img,
        cv::HOGDescriptor& hog,
        float hit_threshold = 1.0,
        cv::Size win_stride = cv::Size(8, 8),
        cv::Size padding = cv::Size(32, 32),
        float scale = 1.05,
        float final_threshold = 2,
        bool use_meanshift_grouping = false) {

        for (const auto& image : images_) {

            auto [bboxes, weights] = detect_.getResults(
                image,
                hog_descriptor_,
                hit_threshold,
                win_stride,
                padding,
                scale,
                final_threshold,
                use_meanshift_grouping);
            std::cout << bboxes.size() << '\n';
        }
    }

private:
    ImageLoader image_loader_;
    ModelLoader model_loader_;
    cv::Ptr<cv::ml::SVM> model_;
    std::vector<cv::Mat> images_;
    cv::HOGDescriptor hog_descriptor_;

    // Model elements
    cv::Mat support_vectors_;
    cv::Mat alpha_;
    cv::Mat svidx_;
    double rho_{};
    std::vector<float> svm_trained_detector_;

    // Detection in multiscale
    MultiScaleDetect detect_;

    void getSupportVectors() {
        support_vectors_ = model_->getSupportVectors();
    }

    void getDecisionFunction() {
        rho_ = model_->getDecisionFunction(0, alpha_, svidx_);
    }

    void getSvmTrainedDetector(bool own_detector = true) {
        svm_trained_detector_.clear();
        if (own_detector) {
            svm_trained_detector_.reserve(support_vectors_.cols + 1);
            for (int j{ 0 }; j < support_vectors_.cols; ++j) {
                svm_trained_detector_[j] = -support_vectors_.at<float>(0, j);
            }
            svm_trained_detector_[support_vectors_.cols] = static_cast<float>(rho_);
            return;
        }
        svm_trained_detector_ = hog_descriptor_.getDefaultPeopleDetector();
    }

    // Set svm detector trained
    void setSvmDetector() {
        hog_descriptor_.setSVMDetector(svm_trained_detector_);
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
        float correct{};
        float tp{}, fp{}, fn{}, tn{};

        for (int i = 0; i < responses_.rows; ++i) {
            float pred = responses_.at<float>(i, 0);
            float actual = train_test_data_->getTestLabels()[i];

            if (pred == actual) {
                correct += 1;
                if (actual == 1) tp++; else tn++;
            }
            else {
                if (actual == 1) fn++; else fp++;
            }
        }

        float accuracy = (correct / responses_.rows) * 100;
        float precision = tp / (tp + fp);
        float recall = tp / (tp + fn);

        std::cout << "Accuracy: " << accuracy << "%\n";
        std::cout << "Precision: " << precision << "\n";
        std::cout << "Recall: " << recall << "\n";
    }

    [[nodiscard]] std::expected<cv::HOGDescriptor, std::string> getHog() {
        if (!train_descriptors_) {
            return std::unexpected("Can't get hog descriptors");
        }
        return train_descriptors_->getHog();
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
    std::filesystem::path path{ "../data/images/pedestrianDataset" };

    SvmClassifier svm;
    svm.loadTrainTestData(path);
    svm.setHogFeatureDescriptor(cv::Size(64, 128));
    svm.train("../data/models/pedestrian.yml");
    svm.predict();
    svm.evaluate();

    //std::filesystem::path path{ "../data/models/eyeGlassClassifierModel.yml" };
    //ModelLoader ml;
    //ml.load_model(path);
    //auto model = ml.get_model();

    return 0;
}
