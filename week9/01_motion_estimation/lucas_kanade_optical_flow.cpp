#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/video/tracking.hpp>
#include <expected>
#include <vector>

class DetectCornersToTrack {
public:
    /// <summary>
    /// Function that allows to set up a Good Features to Track algorithm 
    /// </summary>
    /// <param name="max_corners">Maximum number of corners to be detected</param>
    /// <param name="quality_level">Characterizing the minimal accepted quality of image corners. This value is multiplied by the best corner quality measure</param>
    /// <param name="min_distance">Minimum possible Euclidean distance between the returned corners</param>
    /// <param name="mask">Optional region of interest</param>
    /// <param name="block_size">Size of an average block for computing a derivative covariation matrix over each pixel neighborhood</param>
    /// <param name="use_harris_detector">If true algorithm will use Harris detector instead of Shi-Tomasi</param>
    /// <param name="k">Sets the sensibility of Harris detector</param>
    void set_params(int max_corners, double quality_level, double min_distance,
        cv::Mat mask = {}, int block_size = 3, bool use_harris_detector = false, double k = 0.04) {
        max_corners_ = max_corners;
        quality_level_ = quality_level;
        min_distance_ = min_distance;
        mask_ = mask;
        block_size_ = block_size;
        use_harris_detector_ = use_harris_detector;
        k_ = k;

        is_set = true;
    }

    std::expected<std::vector<cv::Point2f>, std::string> getPoints(const cv::Mat& gray) {
        if (!is_set) {
            return std::unexpected{ "Set parameters first!\n" };
        }

        if (gray.empty()) {
            return std::unexpected{ "Given image is empty!\n" };
        }

        if (gray.channels() != 1) {
            return std::unexpected{ std::format("Mismatch number of channels, expected 1, given: {}", gray.channels()) };
        }

        points_.clear();

        cv::goodFeaturesToTrack(gray,
            points_,
            max_corners_,
            quality_level_,
            min_distance_,
            mask_,
            block_size_,
            use_harris_detector_,
            k_);

        return points_;
    }

private:
    int max_corners_{};
    double quality_level_{};
    double min_distance_{};
    cv::Mat mask_{};
    int block_size_{};
    bool use_harris_detector_{};
    double k_{};

    bool is_set{ false };

    std::vector<cv::Point2f> points_;
};

int main() {


    return 0;
}
