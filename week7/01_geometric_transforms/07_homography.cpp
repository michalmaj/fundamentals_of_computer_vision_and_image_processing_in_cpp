#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <filesystem>
#include <print>
#include <ranges>
#include <string_view>

class Homography {
public:
    Homography(const std::filesystem::path& src, const std::filesystem::path& dst)
        : src_(cv::imread(src.string())), dst_(cv::imread(dst.string())) {
        src_points_.reserve(4);
        dst_points_.reserve(4);
    }
    Homography(const std::filesystem::path& src) : src_(cv::imread(src.string())) {
        dst_ = cv::Mat::zeros(src_.size(), src_.type());
        src_points_.reserve(4);
        dst_points_.reserve(4);
    }

    const auto& getSrc() const { return src_; }
    const auto& getDst() const { return dst_; }
    std::optional<cv::Mat> getWarped() const {
        if (warped_.empty()) {
            return std::nullopt;
        }
        return warped_;
    }

    constexpr std::string srcWindowName() const {
        return "Src";
    }
    constexpr std::string dstWindowName() const {
        return "Dst";
    }

    void setSrcPoints(cv::Point p) {
        if (src_points_.size() >= 4) {
            std::println("You have already 4 source points");
            return;
        }
        src_points_.emplace_back(p);
    }

    void setDstPoints(cv::Point p) {
        if (dst_points_.size() >= 4) {
            std::println("You have already 4 destination points");
            return;
        }
        dst_points_.emplace_back(p);
    }

    void process(bool source_to_dst = false) {
        if (src_points_.size() != 4) {
            emptySrc();
        }

        sort_points(src_points_);

        if (dst_.empty() or dst_points_.size() != 4) {
            emptyDst();
        }

        auto h = cv::findHomography(src_points_, dst_points_);
        cv::warpPerspective(src_, warped_, h, dst_size_);
    }


private:
    cv::Mat src_;
    cv::Mat dst_;
    cv::Mat warped_;
    cv::Size dst_size_;

    std::vector<cv::Point> src_points_;
    std::vector<cv::Point> dst_points_;

    void sort_points(std::vector<cv::Point>& points) {
        cv::Point2f centroid(0, 0);
        for (const auto& p : points) {
            centroid.x += p.x;
            centroid.y += p.y;
        }
        centroid.x /= points.size();
        centroid.y /= points.size();

        std::ranges::sort(points, [&](const cv::Point& a, const cv::Point& b) {
            double angleA = atan2(a.y - centroid.y, a.x - centroid.x);
            double angleB = atan2(b.y - centroid.y, b.x - centroid.x);
            return angleA < angleB;
            });
    }

    //void emptyDst() {
    //    dst_size_ = cv::Size(
    //        std::abs(src_points_[1].x - src_points_[0].x),
    //        std::abs(src_points_[3].y - src_points_[0].y)
    //    );

    //    dst_points_ = { {0, 0}, {dst_size_.width, 0}, {dst_size_.width, dst_size_.height}, {0, dst_size_.height} };
    //}
    void emptyDst() {
        cv::Rect bbox = cv::boundingRect(src_points_);
        dst_size_ = bbox.size();

        dst_points_ = {
            {0, 0},
            {dst_size_.width, 0},
            {dst_size_.width, dst_size_.height},
            {0, dst_size_.height}
        };
    }

    void emptySrc() {
        src_points_ = {
          {0, 0},
          {src_.size().width, 0},
          {src_.size().width, src_.size().height},
          {0, src_.size().height}
        };
    }

};

void setSrcPoints(int event, int x, int y, int flag, void* data) {
    Homography* h = static_cast<Homography*>(data);
  if (event == cv::EVENT_LBUTTONDOWN) {
      h->setSrcPoints({ x, y });
  }
}

int main() {
    std::filesystem::path book2_path{ "../data/images/book2.jpg" };
    std::filesystem::path book1_path{ "../data/images/book1.jpg" };

    if (!std::filesystem::exists(book2_path)) {
        std::cerr << std::format("Can't load an image from: {}", book2_path.string());
        return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(book1_path)) {
        std::cerr << std::format("Can't load an image from: {}", book1_path.string());
        return EXIT_FAILURE;
    }

    Homography h1{ book2_path, book1_path };
    h1.setSrcPoints({ 141, 131 });
    h1.setSrcPoints({ 480, 159 });
    h1.setSrcPoints({ 493, 630 });
    h1.setSrcPoints({ 64, 601 });

    while (true) {
        auto c = cv::waitKey(10);
        if (c == 'q') {
            break;
        }

        if (c == 'p') {
            h1.process();
        }

        cv::imshow(h1.srcWindowName(), h1.getSrc());
        cv::imshow(h1.dstWindowName(), h1.getDst());
        if (h1.getWarped().has_value()) {
            cv::imshow("Warped", h1.getWarped().value());
        }
    }



    cv::destroyAllWindows();

    return 0;
}
