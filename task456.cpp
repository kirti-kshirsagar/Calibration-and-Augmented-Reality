/*
Saikiran Juttu & Kirti Kshirsagar | 16th March 2024
Project-4 : Calibration and Augmented Reality
This program reads the camera calibration parameters from a yml file and prints out the current position of the camera.
Projects a 3D axes at the origin and creates a virtual object on the image space.
Contains task 1, task 2 and task 3
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/calib3d.hpp>
int main() {
    // Read camera calibration parameters from this yml file
    cv::FileStorage fs("/media/sakiran/Internal/2nd Semester/PRCV/Calibration-and-Augmented-Reality/intrinsic_parameters.yml", cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open intrinsic parameters file." << std::endl;
        return -1;
    }
    cv::Mat camera_matrix, distortion_coeffs;
    fs["Camera_Matrix"] >> camera_matrix;
    fs.release();

    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) { // Check if we succeeded
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }

    cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);

    // Define the size of the chessboard
    cv::Size chessboardSize(9, 6); // The size of the chessboard (9x6)

    // Define square size
    float squareSize = 1.0f; // Example: 1.0 unit (e.g., 1 centimeter)

    // Initialize vector to store object points
    std::vector<cv::Point3f> objectPoints;

    // Iterate over each row and column of the checkerboard
    for (int i = 0; i < chessboardSize.height; i++) {
        for (int j = 0; j < chessboardSize.width; j++) {
            // Calculate 3D coordinates of the corner
            cv::Point3f corner(j * squareSize, i * squareSize, 0.0f);
            // Add the corner to the vector of object points
            objectPoints.push_back(corner);
        }
    }
      // Define 3D points for designing a virtual object
    std::vector<cv::Point3f> designPoints;
    designPoints.push_back(cv::Point3f(1, 0, 0));        
    designPoints.push_back(cv::Point3f(0, 0, -10));       
    designPoints.push_back(cv::Point3f(2, 0, -2));        
    designPoints.push_back(cv::Point3f(2, 0, -8));     
    designPoints.push_back(cv::Point3f(4, 0, -2));       
    designPoints.push_back(cv::Point3f(4, 0, -8));     
    designPoints.push_back(cv::Point3f(0, 2, -2));        
    designPoints.push_back(cv::Point3f(0, 2, -8));        
    designPoints.push_back(cv::Point3f(0, 4, -2));     
    designPoints.push_back(cv::Point3f(0, 4, -8));       
    // Define lines connecting points to form the design
    std::vector<std::pair<int, int>> designLines = {
        {0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3}, {2, 4}, {3, 5}, {4, 5},
    {2, 6}, {3, 7}, {4, 8}, {5, 9}, {6, 7}, {6, 8}, {7, 9}, {8, 9}
    };
    while (true) {
        cv::Mat frame;
        cap >> frame; // Capture a new frame
        if (frame.empty()) break; // Check if we succeeded

        // Undistort the frame
        cv::Mat undistorted;
        cv::undistort(frame, undistorted, camera_matrix, distortion_coeffs);

        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(undistorted, gray, cv::COLOR_BGR2GRAY);

        // Detect target (e.g., another chessboard)
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, chessboardSize, corners);

        if (found) {
            // Draw detected target
            cv::drawChessboardCorners(undistorted, chessboardSize, corners, found);

            // Ensure we have enough points for solvePnP
            if (objectPoints.size() < 4 || corners.size() < 4) {
                std::cerr << "Insufficient points for solvePnP" << std::endl;
                continue;
            }

            // Calculate rotation and translation using solvePnP
            cv::Mat rvec, tvec;
            cv::solvePnP(objectPoints, corners, camera_matrix, distortion_coeffs, rvec, tvec);
            // Print rotation and translation
            std::cout << "Rotation Vector:\n" << rvec << std::endl;
            std::cout << "Translation Vector:\n" << tvec << std::endl;
            // Draw coordinate axes on the target
            float axisLength = 2.0f; // Length of the axes
            std::vector<cv::Point3f> axisPoints;
            axisPoints.push_back(cv::Point3f(0, 0, 0));  // Origin
            axisPoints.push_back(cv::Point3f(axisLength, 0, 0));  // X-axis end
            axisPoints.push_back(cv::Point3f(0, -axisLength, 0));  // Y-axis end
            axisPoints.push_back(cv::Point3f(0, 0, -axisLength));  // Z-axis end

            std::vector<cv::Point2f> projectedaxisPoints;
            cv::projectPoints(axisPoints, rvec, tvec, camera_matrix, distortion_coeffs, projectedaxisPoints);

            // Draw X-axis
            cv::arrowedLine(undistorted, projectedaxisPoints[0], projectedaxisPoints[1], cv::Scalar(255, 0, 0), 2);
            // Draw Y-axis
            cv::arrowedLine(undistorted, projectedaxisPoints[0], projectedaxisPoints[2], cv::Scalar(0, 255, 0), 2);
            // Draw Z-axis
            cv::arrowedLine(undistorted, projectedaxisPoints[0], projectedaxisPoints[3], cv::Scalar(0, 0, 255), 2);
            // Project 3D points onto the image plane
            std::vector<cv::Point2f> projectedPoints;
            cv::projectPoints(objectPoints, rvec, tvec, camera_matrix, distortion_coeffs, projectedPoints);
            // Draw projected points on the image
            for (const auto& point : projectedPoints) {
                cv::circle(undistorted, point, 3, cv::Scalar(0, 255, 0), cv::FILLED);
            }
            // Project 3D points of the design onto the image plane
            std::vector<cv::Point2f> projecteddesignPoints;
            cv::projectPoints(designPoints, rvec, tvec, camera_matrix, distortion_coeffs, projecteddesignPoints);

            // Draw lines representing the design
            for (const auto& line : designLines) {
                cv::line(undistorted, projecteddesignPoints[line.first], projecteddesignPoints[line.second], cv::Scalar(0, 255, 0), 2);
            }
        }

        // Display the frame
        cv::imshow("Video", undistorted);

        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q') break; // Quit on 'q'
    }

    // Clean up
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
