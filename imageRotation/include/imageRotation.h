#pragma once
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
cv::Point2f twoLinesIntersectionPoint(cv::Point2f& hori1, cv::Point2f& hori2, cv::Point2f& verti1, cv::Point2f& verti2);
std::vector<cv::Point2f> quadrilateralToRectangle(cv::Point2f& ul, cv::Point2f& ur, cv::Point2f& ll, cv::Point2f& lr);
float angleAfterPointsRotated(cv::Point2f& pt1, cv::Point2f& pt2, cv::Mat& _transform_matrix, bool x_axis);
void transformForm(cv::Mat& src, cv::Mat& dst, std::vector<std::pair<cv::Point2f, cv::Point2f>> horizontal, std::vector< std::pair<cv::Point2f, cv::Point2f>> vertical);