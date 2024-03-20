#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/calib3d.hpp>

int main() {
    // Read camera calibration parameters from the yml file
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

    cv::VideoCapture cap; // Initialize video capture object
    bool useCamera = true; // Flag to indicate whether to use live camera or pre-captured video
    std::string source; // Variable to store video source

    char key = ' '; // Initialize key to a non-'q' value to enter the loop

    // Define the size of the chessboards
    cv::Size chessboardSize1(9, 6); // The size of the first chessboard
    cv::Size chessboardSize2(6, 6); // The size of the second chessboard

    // Define square size
    float squareSize = 1.0f; // 1.0 unit

    // Initialize vectors to store object points for each checkerboard
    std::vector<cv::Point3f> objectPoints1;
    std::vector<cv::Point3f> objectPoints2;

    // Calculate object points for checkerboard 1
    for (int i = 0; i < chessboardSize1.height; i++) {
        for (int j = 0; j < chessboardSize1.width; j++) {
            cv::Point3f corner(j * squareSize, i * squareSize, 0.0f);
            objectPoints1.push_back(corner);
        }
    }

    // Calculate object points for checkerboard 2
    for (int i = 0; i < chessboardSize2.height; i++) {
        for (int j = 0; j < chessboardSize2.width; j++) {
            cv::Point3f corner(j * squareSize, i * squareSize, 0.0f);
            objectPoints2.push_back(corner);
        }
    }

    // Convert object points to constant arrays
    const cv::Mat objectPointsMat1(objectPoints1);
    const cv::Mat objectPointsMat2(objectPoints2);

    cv::Mat gray;

    bool checkerboard1FoundPrev = false;
    bool checkerboard2FoundPrev = false;

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

        while (true) {
            cv::Mat frame;
            cap >> frame; // Capture a new frame
            if (frame.empty()) break; 

            // Undistort the frame
            cv::Mat undistorted;
            cv::undistort(frame, undistorted, camera_matrix, distortion_coeffs);

            // Convert to grayscale
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            // Detect both checkerboards
            std::vector<cv::Point2f> corners1, corners2;
            bool found1 = cv::findChessboardCorners(gray, chessboardSize1, corners1);
            bool found2 = cv::findChessboardCorners(gray, chessboardSize2, corners2);

            // If both checkerboards were found in the previous frame, retain their identities
            if (checkerboard1FoundPrev && checkerboard2FoundPrev) {
                found1 = true;
                found2 = true;
            }

            if (found1 || found2) {
                // Draw detected checkerboards if found
                if (found1) {
                    if (corners1.size() >= chessboardSize1.width * chessboardSize1.height) {
                        cv::drawChessboardCorners(undistorted, chessboardSize1, corners1, found1);
                        checkerboard1FoundPrev = true;

                        std::cout << "Detecting checkerboard 1" << std::endl;
                        cv::Mat rvec1, tvec1;
                        cv::solvePnP(objectPointsMat1, corners1, camera_matrix, distortion_coeffs, rvec1, tvec1);
                        std::cout << "Rotation Vector for checkerboard 1:\n" << rvec1 << std::endl;
                        std::cout << "Translation Vector for checkerboard 1:\n" << tvec1 << std::endl;
                        // Draw coordinate axes on the image for checkerboard 1
                        float axisLength = 2.0f; // Length of the axes
                        std::vector<cv::Point3f> axisPoints1;
                        axisPoints1.push_back(cv::Point3f(0, 0, 0));  // Origin
                        axisPoints1.push_back(cv::Point3f(axisLength, 0, 0));  // X-axis end
                        axisPoints1.push_back(cv::Point3f(0, -axisLength, 0));  // Y-axis end
                        axisPoints1.push_back(cv::Point3f(0, 0, -axisLength));  // Z-axis end

                        std::vector<cv::Point2f> projectedAxisPoints1;
                        cv::projectPoints(axisPoints1, rvec1, tvec1, camera_matrix, distortion_coeffs, projectedAxisPoints1);

                        // Draw X-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints1[0], projectedAxisPoints1[1], cv::Scalar(255, 0, 0), 2);
                        // Draw Y-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints1[0], projectedAxisPoints1[2], cv::Scalar(0, 255, 0), 2);
                        // Draw Z-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints1[0], projectedAxisPoints1[3], cv::Scalar(0, 0, 255), 2);

                        // Draw pyramid on checkerboard 1
                        std::vector<cv::Point3f> pyramidPoints;
                        pyramidPoints.push_back(cv::Point3f(2, 2, -2));       
                        pyramidPoints.push_back(cv::Point3f(3, 3, 0));     
                        pyramidPoints.push_back(cv::Point3f(1, 3, 0));      
                        pyramidPoints.push_back(cv::Point3f(1, 1, 0));    
                        pyramidPoints.push_back(cv::Point3f(3, 1, 0));      

                        std::vector<cv::Point2f> projectedPyramidPoints;
                        cv::projectPoints(pyramidPoints, rvec1, tvec1, camera_matrix, distortion_coeffs, projectedPyramidPoints);

                        // Draw lines to form the pyramid
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[1], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[2], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[3], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[4], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[1], projectedPyramidPoints[2], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[2], projectedPyramidPoints[3], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[3], projectedPyramidPoints[4], cv::Scalar(0, 255, 255), 2);
                        cv::line(undistorted, projectedPyramidPoints[4], projectedPyramidPoints[1], cv::Scalar(0, 255, 255), 2);
                    }
                } else {
                    // If the number of detected corners is insufficient, reset the flag
                    checkerboard2FoundPrev = false;
                }

                if (found2) {
                    if (corners2.size() >= chessboardSize2.width * chessboardSize2.height) {
                        cv::drawChessboardCorners(undistorted, chessboardSize2, corners2, found2);
                        checkerboard2FoundPrev = true;

                        std::cout << "Detecting checkerboard 2" << std::endl;
                        cv::Mat rvec2, tvec2;
                        cv::solvePnP(objectPointsMat2, corners2, camera_matrix, distortion_coeffs, rvec2, tvec2);
                        std::cout << "Rotation Vector for checkerboard 2:\n" << rvec2 << std::endl;
                        std::cout << "Translation Vector for checkerboard 2:\n" << tvec2 << std::endl;
                        // Draw coordinate axes on the image for checkerboard 2
                        float axisLength = 2.0f; // Length of the axes
                        std::vector<cv::Point3f> axisPoints2;
                        axisPoints2.push_back(cv::Point3f(0, 0, 0));  // Origin
                        axisPoints2.push_back(cv::Point3f(axisLength, 0, 0));  // X-axis end
                        axisPoints2.push_back(cv::Point3f(0, -axisLength, 0));  // Y-axis end
                        axisPoints2.push_back(cv::Point3f(0, 0, -axisLength));  // Z-axis end

                        std::vector<cv::Point2f> projectedAxisPoints2;
                        cv::projectPoints(axisPoints2, rvec2, tvec2, camera_matrix, distortion_coeffs, projectedAxisPoints2);

                        // Draw X-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints2[0], projectedAxisPoints2[1], cv::Scalar(255, 0, 0), 2);
                        // Draw Y-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints2[0], projectedAxisPoints2[2], cv::Scalar(0, 255, 0), 2);
                        // Draw Z-axis
                        cv::arrowedLine(undistorted, projectedAxisPoints2[0], projectedAxisPoints2[3], cv::Scalar(0, 0, 255), 2);

                        // Draw pyramid on checkerboard 2
                        std::vector<cv::Point3f> pyramidPoints;
                        pyramidPoints.push_back(cv::Point3f(2, 2, -2));  
                        pyramidPoints.push_back(cv::Point3f(3, 3, 0));      
                        pyramidPoints.push_back(cv::Point3f(1, 3, 0));    
                        pyramidPoints.push_back(cv::Point3f(1, 1, 0));      
                        pyramidPoints.push_back(cv::Point3f(3, 1, 0));       

                        std::vector<cv::Point2f> projectedPyramidPoints;
                        cv::projectPoints(pyramidPoints, rvec2, tvec2, camera_matrix, distortion_coeffs, projectedPyramidPoints);

                        // Draw lines to form the pyramid
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[1], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[2], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[3], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[0], projectedPyramidPoints[4], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[1], projectedPyramidPoints[2], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[2], projectedPyramidPoints[3], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[3], projectedPyramidPoints[4], cv::Scalar(255, 255, 0), 2);
                        cv::line(undistorted, projectedPyramidPoints[4], projectedPyramidPoints[1], cv::Scalar(255, 255, 0), 2);
                    }
                 } else {
                    // If the number of detected corners is insufficient, reset the flag
                    checkerboard2FoundPrev = false;
                }
            } else {
                // If neither checkerboard is found, reset the flags
                checkerboard1FoundPrev = false;
                checkerboard2FoundPrev = false;
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
