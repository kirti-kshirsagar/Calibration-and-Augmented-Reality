/*
Kirti Kshirsagar | 9th March 2024
Project-4 : Calibration and Augmented Reality
This is the main file for checkerboard.png, Detect and Extract Target Corners from Checkerboard Image.
*/
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
    // Load the image
    cv::Mat image = cv::imread("D:/PRCV/Calibration-and-Augmented-Reality/checkerboard.png", cv::IMREAD_GRAYSCALE);
    cv::Size patternsize(9, 6); // number of centers
    std::vector<cv::Point2f> corner_set; // this will be filled by the detected corners

    // Find the chessboard corners
    bool found = cv::findChessboardCorners(image, patternsize, corner_set,
        cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
        + cv::CALIB_CB_FAST_CHECK);

    // If found, refine the corner positions and draw them
    if (found) {
        cornerSubPix(image, corner_set, cv::Size(11, 11), cv::Size(-1, -1),
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
        drawChessboardCorners(image, patternsize, cv::Mat(corner_set), found);
    }

    // Print corner data
    std::cout << "Number of corners: " << corner_set.size() << std::endl;
    if (!corner_set.empty()) {
        std::cout << "First corner coordinates: " << corner_set[0].x << ", " << corner_set[0].y << std::endl;
    }

    // Show the detected corners in an image
    cv::namedWindow("Detected Corners", cv::WINDOW_NORMAL);
    cv::imshow("Detected Corners", image);
    cv::waitKey(0);

    return 0;
}
