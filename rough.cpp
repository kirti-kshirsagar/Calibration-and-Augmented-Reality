#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>

// Helper function to create the 3D point set for a standard checkerboard
std::vector<cv::Vec3f> create3DChessboardCorners(cv::Size boardSize, float squareSize) {
    std::vector<cv::Vec3f> corners;
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(cv::Vec3f(j * squareSize, i * squareSize, 0.0f));
        }
    }
    return corners;
}

int main() {
    // Load camera calibration parameters from a file
    cv::Mat camera_matrix, distortion_coeffs;
    cv::FileStorage fs("intrinsic_parameters.csv", cv::FileStorage::READ);
    fs["Camera Matrix"] >> camera_matrix;
    fs["Distortion Coefficients"] >> distortion_coeffs;
    fs.release();

    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) { // Check if we succeeded
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }

    cv::Size chessboardSize(9, 6); // The size of the chessboard

    while (true) {
        cv::Mat frame;
        cap >> frame; // Capture a new frame
        if (frame.empty()) break; // Check if we succeeded

        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect corners
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, chessboardSize, corners);

        if (found) {
            // Refine corner locations to sub-pixel accuracy
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));

            // Calculate board pose using solvePnP
            cv::Mat rvec, tvec;
            cv::solvePnP(create3DChessboardCorners(chessboardSize, 1.0), corners, camera_matrix, distortion_coeffs, rvec, tvec);

            // Print out rotation and translation data
            std::cout << "Rotation vector (rvec): " << rvec << std::endl;
            std::cout << "Translation vector (tvec): " << tvec << std::endl;
        }

        // Display the frame
        cv::imshow("Video", frame);

        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q') break; // Quit on 'q'
    }

    // Clean up
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
