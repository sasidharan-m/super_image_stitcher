/* Header file for calibrate_camera.cpp. Contains the include lines */


#ifndef CALIBRATE_CAMERA_H
#define CALIBRATE_CAMERA_H

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

typedef std::vector<std::string> stringvec;

//Function to display the calibration images
void disp_img(const string& , Mat);

//Function to read the files in a given directories
void read_directory(const string& , stringvec&);

//Function that starts the calibration
int start_calibration(stringvec&);


#endif /* end of include guard:  */
