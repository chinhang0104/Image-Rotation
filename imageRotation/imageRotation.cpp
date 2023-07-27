#include <imageRotation.h>
// Find the intersection point of two lines
cv::Point2f twoLinesIntersectionPoint(cv::Point2f& pt1, cv::Point2f& pt2, cv::Point2f& pt3, cv::Point2f& pt4) {
    float x1 = pt1.x, y1 = pt1.y;
    float x2 = pt2.x, y2 = pt2.y;
    float x3 = pt3.x, y3 = pt3.y;
    float x4 = pt4.x, y4 = pt4.y;

    float determinant = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4));

    // Check if the lines are parallel
    if (determinant == 0) {
        throw std::invalid_argument("Horizontal line and vertical line are parallel");
    }

    float x = (((x1 * y2) - (y1 * x2)) * (x3 - x4) - (x1 - x2) * ((x3 * y4) - (y3 * x4))) / determinant;
    float y = (((x1 * y2) - (y1 * x2)) * (y3 - y4) - (y1 - y2) * ((x3 * y4) - (y3 * x4))) / determinant;

    return cv::Point2f(x, y);
}

// Convert a quadrilateral to a rectangle
std::vector<cv::Point2f> quadrilateralToRectangle(cv::Point2f& ul, cv::Point2f& ur, cv::Point2f& ll, cv::Point2f& lr) {
    float centroidX = (ul.x + ur.x + ll.x + lr.x) / 4.0f;
    float centroidY = (ul.y + ur.y + ll.y + lr.y) / 4.0f;

    float halveWidth = (std::abs(ur.x - ul.x) + std::abs(lr.x - ll.x)) / 4;
    float halveHeight = (std::abs(ul.y - ll.y) + std::abs(ur.y - lr.y)) / 4;

    // Handle situation that input points are not following correct orientation
    bool invertedX = false;
    bool invertedY = false;
    if (ur.x < ul.x) { invertedX = true; }
    if (ur.y > lr.y) { invertedY = true; }

    cv::Point2f oUR, oUL, oLR, oLL;

    // Both x and y inverted
    if (invertedX && invertedY) {
        oUR = { centroidX - halveWidth, centroidY - halveHeight };
        oLL = { centroidX + halveWidth, centroidY + halveHeight };
        oLR = { centroidX - halveWidth, centroidY + halveHeight };
        oUL = { centroidX + halveWidth, centroidY - halveHeight };
    }
    // Only x inverted
    else if (invertedX) {
        oUR = { centroidX - halveWidth, centroidY - halveHeight };
        oUL = { centroidX + halveWidth, centroidY - halveHeight };
        oLR = { centroidX - halveWidth, centroidY + halveHeight };
        oLL = { centroidX + halveWidth, centroidY + halveHeight };
    }
    // Only y inverted
    else if (invertedY) {
        oUR = { centroidX + halveWidth, centroidY + halveHeight };
        oUL = { centroidX - halveWidth, centroidY + halveHeight };
        oLR = { centroidX + halveWidth, centroidY - halveHeight };
        oLL = { centroidX - halveWidth, centroidY - halveHeight };
    }
    // No inversion
    else {
        oUR = { centroidX + halveWidth, centroidY - halveHeight };
        oUL = { centroidX - halveWidth, centroidY - halveHeight };
        oLR = { centroidX + halveWidth, centroidY + halveHeight };
        oLL = { centroidX - halveWidth, centroidY + halveHeight };
    }

    return { oUL, oUR, oLL, oLR };
}

// Calculate the angle between two lines after applying a transformation matrix
float angleAfterPointsRotated(cv::Point2f& pt1, cv::Point2f& pt2, cv::Mat& transformMatrix, bool isHorizontal) {
    // rotate points
    std::vector<cv::Point2f> oriPts = { pt1, pt2 };
    std::vector<cv::Point2f> out_pts(2);
    cv::perspectiveTransform(oriPts, out_pts, transformMatrix);
    // calculate angle
    float slope;
    if (isHorizontal) {
        // slope = dy/dx
        if (out_pts[0].x != out_pts[1].x) { // avoid divide by 0
            slope = std::abs((out_pts[0].y - out_pts[1].y) / (out_pts[0].x - out_pts[1].x));
        }
        else {
            // dx = 0, angle = 90, return PI/2
            return  CV_PI / 2.0f;
        }
    }
    else {
        // slope = dx/dy
        if (out_pts[0].y != out_pts[1].y) { // avoid divide by 0
            slope = std::abs((out_pts[0].x - out_pts[1].x) / (out_pts[0].y - out_pts[1].y));
        }
        else {
            // dy = 0, angle = 90, return PI/2
            return  CV_PI / 2.0f;
        }
    }
    // Consider a right triangle with adjacent = 1, its opposite = slope * adjacent = slope, and
    // hypotenuse = (adjacent^2 + opposite^2)^0.5 = (1 + slope^2)^0.5
    // angle = acos(adjacent/hypotenuse) = acos(1/(1 + slope^2)^0.5)
    return acos(1 / pow((1 + pow(slope, 2)), 0.5));
}

/**
 * @brief Perform perspective correction on the source image
 * @param src OpenCV Mat, original image
 * @param dst OpenCV Mat, rotated image
 * @param horizontal Vector of pairs of points representing horizontal lines
 * @param vertical Vector of pairs of points representing vertical lines
 */
void transformForm(cv::Mat& src, cv::Mat& dst, std::vector<std::pair<cv::Point2f, cv::Point2f>> horizontal, std::vector< std::pair<cv::Point2f, cv::Point2f>> vertical) {
    float min_total_angle = FLT_MAX;
    cv::Mat best_transform_matrix;
    // Add axis to vectors if any has size smaller than 2
#pragma omp parallel  for
    for (int i = horizontal.size(); i < 2; i++) {
        horizontal.push_back(std::pair<cv::Point2f, cv::Point2f> { cv::Point2f(0, i* src.cols), cv::Point2f(src.rows, i* src.cols) });
    }
#pragma omp parallel  for
    for (int i = vertical.size(); i < 2; i++) {
        vertical.push_back(std::pair<cv::Point2f, cv::Point2f> { cv::Point2f(i* src.rows, 0), cv::Point2f(i* src.rows, src.cols) });
    }
#pragma omp parallel  for
    for (int i = 0; i < horizontal.size(); i++) {
        for (int j = 0; j < vertical.size(); j++) {
            for (int ii = i; ii < horizontal.size(); ii++) {
                for (int jj = j; jj < vertical.size(); jj++) {
                    // ul = upper left, ur = upper right, ll = lower left, lr = lower right
                    cv::Point2f ul = twoLinesIntersectionPoint(horizontal[i].first, horizontal[i].second, vertical[j].first, vertical[j].second);
                    cv::Point2f ur = twoLinesIntersectionPoint(horizontal[i].first, horizontal[i].second, vertical[jj].first, vertical[jj].second);
                    cv::Point2f ll = twoLinesIntersectionPoint(horizontal[ii].first, horizontal[ii].second, vertical[j].first, vertical[j].second);
                    cv::Point2f lr = twoLinesIntersectionPoint(horizontal[ii].first, horizontal[ii].second, vertical[jj].first, vertical[jj].second);
                    std::vector<cv::Point2f> transformed_points = quadrilateralToRectangle(ul, ur, ll, lr);
                    std::vector<cv::Point2f> in_pts{ ul, ur, ll, lr };
                    cv::Mat _transform_matrix = cv::getPerspectiveTransform(in_pts, transformed_points);
                    float total_angle = 0;
                    for (int iii = 0; iii < horizontal.size(); iii++) {
                        total_angle += angleAfterPointsRotated(horizontal[iii].first, horizontal[iii].second, _transform_matrix, true);
                    }
                    for (int jjj = 0; jjj < vertical.size(); jjj++) {
                        total_angle += angleAfterPointsRotated(vertical[jjj].first, vertical[jjj].second, _transform_matrix, false);
                    }
                    if (total_angle < min_total_angle) {
                        min_total_angle = total_angle;
                        best_transform_matrix = _transform_matrix;
                    }
                }
            }
        }
    }
    cv::warpPerspective(src, dst, best_transform_matrix, src.size());
}