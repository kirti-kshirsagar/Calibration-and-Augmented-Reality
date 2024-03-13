/*
Kirti Kshirsagar | 13th March 2024
Project-4 : Calibration and Augmented Reality
Contains task 1, task 2 and task 3
*/
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
        // Calibrate the Camera
        if (corner_list.size() >= 5) { // Require at least 5 calibration frames
            // Create the vector of distortion coefficients
            std::vector<double> distortion_coeffs(5, 0.0); // Assuming radial distortion with 5 parameters
            std::vector<cv::Mat> rotations, translations; // Vectors to store the rotation and translation
            // Create the camera matrix
            cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 1, 0, frame.cols / 2, 0, 1, frame.rows / 2, 0, 0, 1);
            // Print camera matrix and distortion coefficients before calibration
            std::cout << "Camera Matrix before calibration:\n" << camera_matrix << std::endl;
            std::cout << "Distortion Coefficients before calibration:" << std::endl;
            for (const auto& coeff : distortion_coeffs) {
                std::cout << coeff << " ";
            }
            std::cout << std::endl;

            // Calibrate the camera using cv::calibrateCamera function
            double reprojection_error = cv::calibrateCamera(point_list, corner_list, frame.size(), camera_matrix, distortion_coeffs, rotations, translations);

            // Print camera matrix and distortion coefficients after calibration
            std::cout << "Camera Matrix after calibration:\n" << camera_matrix << std::endl;
            std::cout << "Distortion Coefficients after calibration:" << std::endl;
            for (const auto& coeff : distortion_coeffs) {
                std::cout << coeff << " ";
            }
            std::cout << std::endl;
            // Print rotations for calibration images
            for (size_t i = 0; i < rotations.size(); i++) {
                std::cout << "Rotation for calibration image " << i + 1 << ":\n" << rotations[i] << std::endl;
            }
            // Print translations for calibration images
            for (size_t i = 0; i < translations.size(); i++) {
                std::cout << "Translation for calibration image " << i + 1 << ":\n" << translations[i] << std::endl;
            }

            // Print final re-projection error
            std::cout << "Final Re-projection Error: " << reprojection_error << std::endl;

            // Save intrinsic parameters to a file
            cv::FileStorage fs("D:/PRCV/Calibration-and-Augmented-Reality/intrinsic_parameters.yml", cv::FileStorage::WRITE);
            fs << "Camera_Matrix" << camera_matrix;
            fs << "Distortion_Coefficients" << distortion_coeffs;
            fs.release();

            // Save rotations and translations to a file
            cv::FileStorage fs_rot_trans("D:/PRCV/Calibration-and-Augmented-Reality/rotations_translations.yml", cv::FileStorage::WRITE);
            fs_rot_trans << "Rotations" << rotations;
            fs_rot_trans << "Translations" << translations;
            fs_rot_trans.release();

        }
    }

    // Clean up
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
