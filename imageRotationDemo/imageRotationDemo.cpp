#include <numeric>
#include <opencv2/opencv.hpp>
#include <imageRotation.h>

// Test for twoLinesIntersectionPoint
void TestTwoLinesIntersectionPoint(cv::Point2f& A, cv::Point2f& B, cv::Point2f& C, cv::Point2f& D) {
	cv::Point2f intersectionPoint = twoLinesIntersectionPoint(A, B, C, D);
	std::cout << "intersectionPoint: \n" << intersectionPoint << std::endl;
}

// Test for quadrilateralToRectangle
void TestQuadrilateralToRectangle(cv::Point2f& A, cv::Point2f& B, cv::Point2f& C, cv::Point2f& D) {
	std::vector<cv::Point2f> rect = quadrilateralToRectangle(A, B, C, D);
	std::cout << "Rectangle: \n" << rect << std::endl;
}

// Test for PerspectiveTransform
void TestPerspectiveTransform(cv::Point2f& A, cv::Point2f& B, cv::Point2f& C, cv::Point2f& D) {
	std::vector<cv::Point2f> oriPts = { A, B, C, D };
	std::vector<cv::Point2f> outPts;
	std::vector<cv::Point2f> rect = quadrilateralToRectangle(A, B, C, D);
	cv::Mat test_image(1000, 1000, CV_8UC3, cv::Scalar(255, 255, 255));
	cv::line(test_image, A, B, cv::Scalar(0, 0, 0), 10);
	cv::line(test_image, B, D, cv::Scalar(0, 0, 0), 10);
	cv::line(test_image, C, D, cv::Scalar(0, 0, 0), 10);
	cv::line(test_image, C, A, cv::Scalar(0, 0, 0), 10);
	cv::Mat transformMatrix = cv::getPerspectiveTransform(oriPts, rect);
	std::cout << "transformMatrix: \n" << transformMatrix << std::endl;

	cv::Mat dst;
	cv::warpPerspective(test_image, dst, transformMatrix, test_image.size());
	cv::perspectiveTransform(oriPts, outPts, transformMatrix);
	std::cout << "outPts: \n" << outPts << std::endl;

	// Test for angleAfterPointsRotated
	cv::Point2f B2 = { B.x, B.y - 10 };
	float ang1 = angleAfterPointsRotated(A, B2, transformMatrix, false);
	std::cout << "angleAfterPointsRotated: \n" << ang1 << std::endl;

	// Display the original and rotated images
	cv::namedWindow("Original", cv::WINDOW_NORMAL);
	cv::imshow("Original", test_image);
	cv::waitKey(0);
	cv::imshow("Rotated", dst);
	cv::waitKey(0);
}

// Test for transformForm
void TestTransformForm(std::vector<std::pair<cv::Point2f, cv::Point2f>>& horizontal, std::vector<std::pair<cv::Point2f, cv::Point2f>>& vertical) {
	cv::Mat srcMat(1000, 1000, CV_8UC3, cv::Scalar(255, 255, 255));
	for (std::pair<cv::Point2f, cv::Point2f> line : horizontal) {
		cv::line(srcMat, line.first, line.second, cv::Scalar(255, 0, 0), 10);
	}
	for (std::pair<cv::Point2f, cv::Point2f> line : vertical) {
		cv::line(srcMat, line.first, line.second, cv::Scalar(0, 255, 0), 10);
	}

	cv::Mat outMat;
	transformForm(srcMat, outMat, horizontal, vertical);

	// Display the original and rotated images
	cv::imshow("Original", srcMat);
	cv::waitKey(0);
	cv::imshow("Rotated", outMat);
	cv::waitKey(0);
}

int main() {
	cv::Point2f A{ 100, 100 };
	cv::Point2f B{ 800, 50 };
	cv::Point2f C{ 150, 500 };
	cv::Point2f D{ 900, 900 };

	// Points for horizonal lines
	cv::Point2f E{ 100, 120 };
	cv::Point2f F{ 800, 130 };
	cv::Point2f EE{ 120, 140 };
	cv::Point2f FF{ 820, 150 };
	cv::Point2f I{ 200, 240 };
	cv::Point2f J{ 950, 260 };

	// Points for vectical lines
	cv::Point2f G{ 130, 880 };
	cv::Point2f H{ 850, 900 };
	cv::Point2f GG{ 150, 900 };
	cv::Point2f HH{ 870, 920 };
	cv::Point2f M{ 200, 50 };
	cv::Point2f N{ 200, 900 };

	std::vector<std::pair<cv::Point2f, cv::Point2f>> horizontal{ {E, F}, { G, H }, { A, B }, { EE, FF }, { GG, HH }};
	std::vector<std::pair<cv::Point2f, cv::Point2f>> vertical{ {A, C}, { E,G }, { F, H }, { EE, GG }, { FF, HH }};

	TestTwoLinesIntersectionPoint(A, B, C, D);
	TestQuadrilateralToRectangle(A, B, C, D);
	TestPerspectiveTransform(A, B, C, D);
	TestTransformForm(horizontal, vertical);
}