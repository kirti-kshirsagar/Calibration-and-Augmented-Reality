# README - Project 4
Name - **KIRTI KSHIRSAGAR** & **SAIKIRAN JUTTU**

- Using Windows 10, VScode, CMake,OpenCV 4.9(recent one) and C++.
- Not using any Time Travel Days
- Extension : Keypress 'p' activates the pre-captured video containing the target.

## This is a Readme file for main.cpp, task456.cpp and task7.cpp file and this includes all the details that are necessary to run the program.

The main.cpp file contains tasks from 1 to 3, task456.cpp file contains tasks from 4 to 6 and task 7 is present in task7.cpp.

### Task-1
-  In this task, we build a system for detecting a target and extracting target corners from live camera feed. We use the provided checkerboard as target image. Here, we have printed out the number of corners, along with the coordinates of the first corner.

### Task-2
- This task lets the user select images for calibration. In this task, upon pressing **s**, we let the user specify that a particular image should be used for the calibration and save the corner locations and the corresponding 3D world points.

### Task-3
- In this task, we calibrated the camera if the user has selected minimum of 5 images for calibration. We used **cv::calibrateCamera** function to generate the calibration and printed camera matrix and distortion coefficients before and after the calibration. We also calculated and printed Re-projection Error after the calibration has been done. 
- We are saving the rotation and translation associated with each calibration image in a file. Also, the intrinsic parameters are getting saved to a file for furthur tasks.

### Task-4

### Task-5

### Task-6

### Task-7

# Extension:
For the extensions, we have done the following tasks:
- Task 1 : We have enabled our system to use pre-captured video sequences with targets and demonstrated inserting virtual objects into the scenes.
    * The conversion from live feed to the pre-captured video takes place upon keypress **p**. [Attached Demo of this task](https://drive.google.com/file/d/103mUiSgL6q1pJ2LhHZq8z1r2gVORhIdA/view?usp=sharing)
- Task 2 : Test out several different cameras and compare the calibrations and quality of the results. 
    * For this task, we have compared the results from my **laptop integrated web cam** and **external web cam**.


 