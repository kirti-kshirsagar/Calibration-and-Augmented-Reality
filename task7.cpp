/*
Saikiran Juttu & Kirti Kshirsagar | 18th March 2024
Project-4 : Calibration and Augmented Reality
This program detects and shows Robust Features such as Harris corners in a live camera feed.
*/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0); // Open default camera
    if (!cap.isOpened()) {
        cerr << "Error: Unable to open camera" << endl;
        return -1;
    }

    // Parameters for Harris corner detection, change them and see the effects acccordingly
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;
    int threshold = 150;
    //int threshold = 200;

    Mat frame, grayFrame, dst, dst_norm, dst_norm_scaled;
    while (true) {
        cap >> frame; // Capture frame from camera
        if (frame.empty()) //error check
            break;

        // Convert frame to grayscale
        cvtColor(frame, grayFrame, COLOR_BGR2GRAY);

        // Detect Harris corners
        dst = Mat::zeros(frame.size(), CV_32FC1);
        cornerHarris(grayFrame, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

        // Normalizing and scaling to enhance the result
        normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
        convertScaleAbs(dst_norm, dst_norm_scaled);

        // Draw circles around corners
        for (int i = 0; i < dst_norm.rows; i++) {
            for (int j = 0; j < dst_norm.cols; j++) {
                if (static_cast<int>(dst_norm.at<float>(i, j)) > threshold) {
                    circle(frame, Point(j, i), 5, Scalar(0, 0, 255), 2, 8, 0);
                    //rectangle(frame, Point(j - 5, i - 5), Point(j + 5, i + 5), Scalar(0, 0, 255), 2, 8, 0);

                }
            }
        }

        // Display frame with Harris corners
        imshow("Harris Corners", frame);
        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q') break; // Quit on 'q'
    }
    // Clean up
    cap.release();
    destroyAllWindows();

    return 0;
}