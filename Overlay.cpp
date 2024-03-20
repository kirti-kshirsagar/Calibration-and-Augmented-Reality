#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/calib3d.hpp>

int main() {
    // Read camera calibration parameters from this yml file
    cv::FileStorage fs("D:/PRCV/Calibration-and-Augmented-Reality/intrinsic_parameters.yml", cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open intrinsic parameters file." << std::endl;
        return -1;
    }
    cv::Mat camera_matrix;
    std::vector<double> distortion_coeffs;
    
    fs["Camera_Matrix"] >> camera_matrix;
    fs["Distortion_Coefficients"] >> distortion_coeffs;
    fs.release();

    // Convert the distortion coefficients vector to an OpenCV matrix
    cv::Mat distortion_coeffs_mat(1, distortion_coeffs.size(), CV_64F);
    for (size_t i = 0; i < distortion_coeffs.size(); i++) {
        distortion_coeffs_mat.at<double>(0, i) = distortion_coeffs[i];
    }

    cv::VideoCapture cap; // Initialize video capture object
    bool useCamera = true; // Flag to indicate whether to use live camera or pre-captured video
    std::string source; // Variable to store video source

    char key = ' '; // Initialize key to a non-'q' value to enter the loop

    // Load the image to overlay on the target
    cv::Mat overlayImage = cv::imread("D:/PRCV/Calibration-and-Augmented-Reality/robo.jpg");
    if (overlayImage.empty()) {
        std::cerr << "Failed to load overlay image." << std::endl;
        return -1;
    }

    while (key != 'q') {
        if (useCamera) {
            // Use live camera feed
            cap.open(0); // Open the default camera
            source = "Live Camera";
        } else {
            // Use pre-captured video feed
            cap.open("D:/PRCV/cc.mp4"); // Open the pre-captured video
            source = "Pre-captured Video";
        }

        if (!cap.isOpened()) { // Check if we succeeded in opening the capture
            std::cerr << "Could not open " << source << std::endl;
            return -1;
        }

        cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);

        // Define the size of the chessboard
        cv::Size chessboardSize(9, 6); // The size of the chessboard (9x6)

        // Define square size
        float squareSize = 1.0f; // 1.0 unit

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
        designPoints.push_back(cv::Point3f(1, 1, 0));      
        designPoints.push_back(cv::Point3f(4, 1, 0));    
        designPoints.push_back(cv::Point3f(4, 4, 0));      
        designPoints.push_back(cv::Point3f(1, 4, 0));    
        designPoints.push_back(cv::Point3f(1, 2, 0));   
        designPoints.push_back(cv::Point3f(3, 2, 0));     
        designPoints.push_back(cv::Point3f(3, 3, 0));       
        designPoints.push_back(cv::Point3f(1, 3, 0));     
        designPoints.push_back(cv::Point3f(1, 1, -2));      
        designPoints.push_back(cv::Point3f(4, 1, -2));      
        designPoints.push_back(cv::Point3f(4, 4, -2));        
        designPoints.push_back(cv::Point3f(1, 4, -2));     
        designPoints.push_back(cv::Point3f(1, 2, -2));      
        designPoints.push_back(cv::Point3f(3, 2, -2));     
        designPoints.push_back(cv::Point3f(3, 3, -2));       
        designPoints.push_back(cv::Point3f(1, 3, -2));      
        // Define lines connecting points to form the design
        std::vector<std::pair<int, int>> designLines = {
            {0, 1}, {1, 2}, {2, 3}, {0, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 3}, {8, 9},
        {9, 10}, {10, 11}, {8, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 11}, {0, 8},
        {1, 9}, {2, 10}, {3, 11}, {4, 12}, {5, 13}, {6, 14}, {7, 15}
        };

        while (true) {
            cv::Mat frame;
            cap >> frame; // Capture a new frame
            if (frame.empty()) break; 

            // Undistort the frame
            cv::Mat undistorted;
            cv::undistort(frame, undistorted, camera_matrix, distortion_coeffs);

            // Convert to grayscale
            cv::Mat gray;
            cv::cvtColor(undistorted, gray, cv::COLOR_BGR2GRAY);

            // Detect target
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

                // Project 3D points onto the image plane
                std::vector<cv::Point2f> projectedPoints;
                cv::projectPoints(objectPoints, rvec, tvec, camera_matrix, distortion_coeffs, projectedPoints);
                
                // Draw projected points on the image
                for (const auto& point : projectedPoints) {
                    cv::circle(undistorted, point, 3, cv::Scalar(0, 255, 0), cv::FILLED);
                }

                // Overlay the image onto the chessboard area
                // Get the minimum and maximum x, y coordinates of the corners
                float min_x = FLT_MAX, min_y = FLT_MAX, max_x = 0, max_y = 0;
                for (const auto& corner : corners) {
                    min_x = std::min(min_x, corner.x);
                    min_y = std::min(min_y, corner.y);
                    max_x = std::max(max_x, corner.x);
                    max_y = std::max(max_y, corner.y);
                }

                // Calculate the region of interest (ROI) for the overlay image
                cv::Rect roi(min_x, min_y, max_x - min_x, max_y - min_y);
                
                // Resize the overlay image to fit within the ROI
                cv::Mat resizedOverlay;
                cv::resize(overlayImage, resizedOverlay, roi.size());

                // Copy the resized overlay image to the frame
                cv::Mat frame_roi = undistorted(roi);
                resizedOverlay.copyTo(frame_roi);
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

            key = static_cast<char>(cv::waitKey(30));
            if (key == 'q') break; // Quit on 'q'
            else if (key == 'p') {
                // Toggle between live camera and pre-captured video
                useCamera = !useCamera;
                break;
            }
        }

        // Clean up
        cap.release();
        cv::destroyAllWindows();
    }

    return 0;
}
