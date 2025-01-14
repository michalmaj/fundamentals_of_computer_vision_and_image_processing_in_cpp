#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <vector>

class Equalizer {
protected:
    Equalizer(const cv::Mat& img) : bgr_(img) {
        if (img.channels() != 3 or img.type() != CV_8UC3) {
            throw std::runtime_error("Image must be in bgr format!\n");
        }
    }
    cv::Mat bgr_;
    cv::Mat hsv_;
    std::vector<cv::Mat> channels_;

    void convertToHsv() {
        cv::cvtColor(bgr_, hsv_, cv::COLOR_BGR2HSV);
    }

    void splitChannels() {
        cv::split(hsv_, channels_);
    }

    void mergeChannels() {
        cv::merge(channels_, hsv_);
    }

    void convertToBgr() {
        cv::cvtColor(hsv_, bgr_, cv::COLOR_HSV2BGR);
    }
};

class HistogramEqualizer: public Equalizer{
public:
    HistogramEqualizer(const cv::Mat& img) : Equalizer(img.clone()) {}

    void process() {
        convertToHsv();
        splitChannels();
        equalizeHistogram();
        mergeChannels();
        convertToBgr();
    }

    const auto& getImage() const { return bgr_; }
    const auto& getWindowName() const { return window_name_; }
private:
    std::string window_name_{ "Histogram Equalized" };
    void equalizeHistogram() {
        cv::equalizeHist(channels_.at(2), channels_.at(2));
    }
};

class ClaheEqualizer : public Equalizer {
public:
    ClaheEqualizer(const cv::Mat& img) : Equalizer(img.clone()) {}

    void process() {
        convertToHsv();
        splitChannels();
        equalizeHistogram();
        mergeChannels();
        convertToBgr();
    }

    const auto& getImage() const { return bgr_; }
    const auto& getWindowName() const { return window_name_; }
private:
    std::string window_name_{ "CLAHE Equalized" };
    void equalizeHistogram() {
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        clahe->apply(channels_.at(2), channels_.at(2));
    }
};

int main() {
    // Path to an image 
    std::string path{ "../data/images/night-sky.jpg" };

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
    HistogramEqualizer he{ img };
    he.process();

    ClaheEqualizer ce{ img };
    ce.process();


    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow(he.getWindowName(), cv::WINDOW_NORMAL);
    cv::namedWindow(ce.getWindowName(), cv::WINDOW_NORMAL);

    cv::imshow("Original", img);
    cv::imshow(he.getWindowName(), he.getImage());
    cv::imshow(ce.getWindowName(), ce.getImage());
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
