/*Header file for undistort_images.cpp */

#ifndef UNDISTORT_IMAGES_H
#define UNDISTORT_IMAGES_H

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

//Function that reads the intrinsic camera matrix and the distortion params from the given XML
void get_dist_params( Mat&, Mat&, const string&);

//Function that undistorts the given images
void undistort_images(const string& img_path);

#endif /* end of include guard:  */
