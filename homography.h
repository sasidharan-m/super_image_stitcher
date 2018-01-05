/*Header file for homography.cpp */

#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/stitching.hpp"

using namespace cv;
using namespace cv::xfeatures2d;


using namespace cv;
using namespace std;

typedef std::vector<std::string> stringvec;


//Function that calculates the pairwise homographies of the given images
void calc_homographies(stringvec& );

//Function that relates the homographies to the first image
void relate_homography(vector<Mat>& , vector<Mat>& );

//Function that wraps the images
void wrap_images(stringvec&);

//Function that stitches the images
void stitch_images(Mat&, vector<Mat>& );

//Function to rotate the supre image
void rotate_super_image(Mat&);

#endif
