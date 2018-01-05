/*C++ file that calibrates the camera */

#include "calibrate_camera.h"

/* Function to display an image with a window
Usage:
Parameters:
window_name = a string that denotes the window name
img = matrix of image to be displayed
Returns:
returns nothing
*/
void disp_img(const string& window_name, Mat img)
{
	//Checking if the matix is empty
	if (img.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
	}
	//Creating a named window
	namedWindow(window_name, CV_WINDOW_NORMAL);
	//Displaying the created window
	imshow(window_name, img);
	//Waiting for a keystroke or 500 ms (Whichever one occurs first)
	waitKey(500);
	cout << "Closing Image..." << endl;
	//Destroying the created window
	destroyWindow(window_name);

}

/*Function to read a given directory
Usage:
Parameters:
name = string that has the name of the directory
v = a vector of strings that is used to store the files in the directory
Returns:
returns nothing
*/

void read_directory(const string& name, stringvec& v)
{
		//Initializing a file pointer to point to the directory that has to be opened
		DIR* dirp = opendir(name.c_str());
		//Initializing struc to store the attributes of a file
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
    	if (strcmp(dp->d_name, ".") == 0)
    		continue;
    	if (strcmp(dp->d_name, "..") == 0)
    		continue;
    	string str1(dp->d_name);
			//Check if the file is a JPEG image
    	if (str1.find(".jpg") == string::npos)
    		continue;
		//Appened the name of the JPEG image to the vector of strings v
		v.push_back(name + (dp->d_name));
    }
		//Closing the directory
		closedir(dirp);
    //Sorting the vector of strings v, so that the files are in alphabetical order
  	std::sort(v.begin(), v.end());
}


/*Function that starts the calibration
Usage:
Parameters:
vector<string> v: a vector of strings that stores the path to the images used for calibration
Returns:
returns nothing
*/

int start_calibration(stringvec& v)
{
  int numBoards = 36;
  int numCornersHor = 7;
  int numCornersVer = 7;

	/*
  printf("Enter number of corners along width: ");
  scanf("%d", &numCornersHor);

  printf("Enter number of corners along height: ");
  scanf("%d", &numCornersVer);

  printf("Enter number of boards: ");
  scanf("%d", &numBoards);
	*/

  int numSquares = numCornersHor * numCornersVer;
  Size board_sz = Size(numCornersHor, numCornersVer);

  vector<vector<Point3f>> object_points;
  vector<vector<Point2f>> image_points;

  vector<Point2f> corners;
  int successes=0;

  Mat image;
  Mat gray_image;

  vector<Point3f> obj;
  for(int j=0;j<numSquares;j++)
      obj.push_back(Point3f(j/numCornersHor, j%numCornersHor, 0.0f));
	//For each image do the calibration
  for(auto  & element : v)
  {
      image = imread(element, CV_LOAD_IMAGE_UNCHANGED);
      cvtColor(image, gray_image, CV_BGR2GRAY);
			//check if the corners of the chess board is detected
      bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
			//if detected do the calibration
      if(found)
      {
          cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
          drawChessboardCorners(gray_image, board_sz, corners, found);
      }
			//display the detected corners
      namedWindow("win1", WINDOW_NORMAL);
      resizeWindow("win1", 1000, 1000);
      namedWindow("win2", WINDOW_NORMAL);
      resizeWindow("win2", 1000, 1000);
      moveWindow("win2", 1100,0);
      imshow("win1", image);
      imshow("win2", gray_image);


      int key = waitKey(1000);
			//if the Esc key is pressed break all operations and exit
      if(key==27)
          return 0;

      if(found!=0)
      {
          image_points.push_back(corners);
          //cout << "corners" << corners << '\n';
          //cout << "obj:" << obj << '\n';
          object_points.push_back(obj);
          printf("Snap stored!\n");

          successes++;
					//check if enoungh points have been detected for calibration
          if(successes>=numBoards)
          {
                cout << "Got enough data points. Exiting loop...\n";
                break;
          }
      }
  }

  Mat intrinsic = Mat(3, 3, CV_32FC1);
  Mat distCoeffs;
  vector<Mat> rvecs;
  vector<Mat> tvecs;

  intrinsic.ptr<float>(0)[0] = 1;
  intrinsic.ptr<float>(1)[1] = 1;

  calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);

  Mat imageUndistorted;
  for(auto  & element : v)
  {
      image = imread(element, CV_LOAD_IMAGE_UNCHANGED);
      undistort(image, imageUndistorted, intrinsic, distCoeffs);

      imshow("win1", image);
      imshow("win2", imageUndistorted);

      waitKey(100);
  }
  destroyAllWindows();
  string param_filename = "camera_params.xml";
  FileStorage fs(param_filename, FileStorage::WRITE);
  fs << "camera_matrix" << intrinsic;
  fs << "distortion_coefficients"  << distCoeffs;
  fs.release();
  return 0;
}
