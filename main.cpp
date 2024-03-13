/*
Kirti Kshirsagar | 9th March 2024
Project-4 : Calibration and Augmented Reality
Contains task 1 and task 2
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

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
    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) { // Check if we succeeded
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }

    cv::Size chessboardSize(9, 6); // The size of the chessboard
    float squareSize = 1.0; // Set this to your actual square size
    std::vector<std::vector<cv::Point2f>> corner_list; // 2D points in image space
    std::vector<std::vector<cv::Vec3f>> point_list; // 3D points in world space

    std::vector<cv::Vec3f> point_set = create3DChessboardCorners(chessboardSize, squareSize);

    cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);

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
            // Refine corner locations to sub-pixel accuracy and draw them
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
            cv::drawChessboardCorners(frame, chessboardSize, corners, found);

            std::cout << "Number of corners: " << corners.size() << std::endl;
            std::cout << "First corner: " << corners[0] << std::endl;
        }

        // Display the frame
        cv::imshow("Video", frame);

        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q') break; // Quit on 'q'
        if (key == 's' && found) { // Save on 's' if corners found
            corner_list.push_back(corners);
            point_list.push_back(point_set);
            // Saving calibration images
            cv::imwrite("D:/PRCV/Calibration-and-Augmented-Reality/calibrationImg/calibration_frame_" + std::to_string(corner_list.size()) + ".jpg", frame);
            std::cout << "Saved current frame's corner data." << std::endl;
        }
    }

    // Clean up
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
