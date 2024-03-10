/*
Kirti Kshirsagar | 9th March 2024
Project-4 : Calibration and Augmented Reality
This Detects and Extracts Target Corners from Checkerboard Image using the camera.
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    cv::VideoCapture cap(0); // Open the default camera
    if(!cap.isOpened()) { // Check if we succeeded
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }

    cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);

    while(true) {
        cv::Mat frame;
        cap >> frame; // Capture a new frame
        if(frame.empty()) break; // Check if we succeeded

        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect corners
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, cv::Size(9, 6), corners);

        // Draw corners and output data if found
        if(found) {
            cv::drawChessboardCorners(frame, cv::Size(9, 6), cv::Mat(corners), found);
            std::cout << "Number of corners: " << corners.size() << std::endl;
            std::cout << "First corner: " << corners[0] << std::endl;
        }

        // Display the frame
        cv::imshow("Video", frame);

        // Break the loop if 'q' is pressed
        if(cv::waitKey(30) == 'q') break;
    }

    // Clean up
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
