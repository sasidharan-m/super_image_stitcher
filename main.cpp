/*Main cpp file that integrates all stuff to be done */

//include all the header files necessary
#include "calibrate_camera.h"
#include "undistort_images.h"
#include "homography.h"

//main function
int main(int argc, char* argv[])
{
  int res = 0;
  //chack if any arguments are given
  if (argc != 1)
  {
    cout << "No arguments required. Setting in default.xml is imported.\n";
    return 1;
  }
  else
  {
    //string vector to hold the images paths
    stringvec v;
    //path of directory containing the images
    string path = "calibration_pics/";
    //reading the images in the specified directory
    read_directory(path, v);
    //printing the detected images paths
    cout << "Detected Images for calibration:\n";
    for (auto & element: v)
    {
      cout << element << '\n';
    }
    cout << "Starting camera calibration..\n";
    res = (start_calibration(v));
  }
  if (!res)
  {
    //get_dist_params(intrinsic, distCoeffs, "camera_params.xml");
    undistort_images("pictures/");
    //string vector to hold paths of the undistorted images
    stringvec v1;
    read_directory("undistorted_images/", v1);
    cout << "Calculating the homography matrices...\n";
    calc_homographies(v1);
  }
}
