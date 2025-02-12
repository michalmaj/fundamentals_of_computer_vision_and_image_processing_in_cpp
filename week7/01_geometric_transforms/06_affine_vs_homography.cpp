#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <vector>
#include <array>

int main() {
    // Create an empty black image
    cv::Mat img(200, 200, CV_32FC3, cv::Scalar(0, 0, 0));

    // Add a blue square in the middle
    int cx{ img.size().width / 2 };
    int cy{ img.size().height / 2 };
    cv::Point tl{ cx - cx / 2, cy - cy / 2 };
    img(cv::Range(tl.y, tl.y + cy), cv::Range(tl.x, tl.x + cx)).setTo(cv::Scalar(1, 0.6, 0.2));

    // Estimate transformation by given points (4 points in this case) for linear transform
    std::vector<cv::Point2f> src_points{ {50, 50}, {50, 149}, {149, 149}, {149, 50} };
    std::vector<cv::Point2f> dst_points{ {68,45}, {76,155}, {183,135}, {176,27} };

    cv::Mat estimated{ cv::estimateAffine2D(src_points, dst_points) };

    cv::Mat linear_transform;
    cv::warpAffine(img, linear_transform, estimated, img.size());

    // Draw a trapezoid
    cv::Mat img_trapezoid(200, 200, CV_32FC3, cv::Scalar(0, 0, 0));
    std::vector<cv::Point> trapezoid_points = { {75,50}, {50,149}, {149,149}, {124,50} };
    cv::fillConvexPoly(img_trapezoid, trapezoid_points, cv::Scalar(1.0, 0.6, 0.2), cv::LINE_AA);

    // Try to estimate transformation using linear function
    estimated = cv::estimateAffine2D(src_points, trapezoid_points);

    cv::Mat non_linear_bad;
    cv::warpAffine(img, non_linear_bad, estimated, img.size());


    // Better version with homography
    cv::Mat homography{ cv::findHomography(src_points, trapezoid_points) };

    cv::Mat non_linear_correct;
    cv::warpPerspective(img, non_linear_correct, homography, img.size());



    cv::imshow("Original", img);
    cv::imshow("Linear Transform", linear_transform);
    cv::imshow("Trapezoid", img_trapezoid);
    cv::imshow("Bad estimation", non_linear_bad);
    cv::imshow("Correct estimation", non_linear_correct);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
