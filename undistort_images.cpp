/*C++ program that undistorts the given images */

#include "undistort_images.h"
#include "calibrate_camera.h"


/*Function that reads the intrinsic camera matrix and the distortion params from the given XML
Usage:
Parameters:
Mat intrinsic: stores the values of the insttrinsic camera matrix
Mat distCoeffs: stores the distortion parameters
const string param_filename: stores the path to the XML file containing the intrinsic matrix and distortion parameters
Returns:
returns nothing the matrices are passed by reference
*/

void get_dist_params( Mat& intrinsic, Mat& distCoeffs, const string& param_filename)
{
  FileStorage fs(param_filename, FileStorage::READ);
  fs["camera_matrix"] >> intrinsic;
  fs["distortion_coefficients"] >> distCoeffs;
  cout<< "The intrinsic camera matrix is:\n" << intrinsic << endl << "The distortion coefficients are:\n" << distCoeffs << endl;
  fs.release();
}


/*Function that undistorts the given images
Usage:
Parameters:
const string img_path: stores the path to the directory conntaining the images used for calibration of the camera
Returns:
returns nothing
*/
void undistort_images(const string& img_path)
{
  Mat intrinsic = Mat(3, 3, CV_32FC1);
  Mat distCoeffs;
  get_dist_params(intrinsic, distCoeffs, "camera_params.xml");
  //string vector to hold the images paths
  stringvec v;
  read_directory(img_path, v);
  char i = 'a';
  string name;
  //for each image in the directory
  for (auto & element: v)
  {
    cout << "Undistorting image: " << element << endl;
    //read the image
    Mat image = imread(element, CV_LOAD_IMAGE_UNCHANGED);
    //declaring a mat object to store the undistorted image
    Mat imageUndistorted;
    //calling function that undistorts the image
    undistort(image, imageUndistorted, intrinsic, distCoeffs);
    //displays the result
    namedWindow("distorted image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
    resizeWindow("distorted image", 1300, 1000);
    namedWindow("undistorted image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
    resizeWindow("undistorted image", 1300, 1000);
    moveWindow("undistorted image", 1600,0);
    imshow("distorted image", image);
    imshow("undistorted image", imageUndistorted);
    name = "undistorted_images/img" + string(1,i) + ".jpg";
    //store the undistorted image
    imwrite(name, imageUndistorted);
    i ++;
    waitKey(1000);
    destroyAllWindows();
  }
  //destroyAllWindows();
  cout << "Finished undistorting...\n";
}
