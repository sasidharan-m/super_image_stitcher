/*C++ file that calculates the pairwise hokographies of the given images */

#include "homography.h"



/*Function that relates the homographies to the first image
Usage:
Parameters:
vector<Mat> h1: a vector of matrices that has the homography matrices that relate the adjacent images
vector<Mat> h2: a vector of matrices that has the homography matrices that relate each image with the first image
Returns:
returns nothing, both matrices are passed by reference
*/
void relate_homography(vector<Mat>& h1, vector<Mat>& h2)
{
  if(h1.size() < 1)
  {
    cout << "No matrices given.\n";
    return;
  }
  Mat tmp = Mat::eye(3,3,CV_64FC1);

  //creating and opening a xml file to store homography matrix values realted to the first image
  string homography_mat_file_2 = "related_homography_matrices.xml";
  FileStorage fs(homography_mat_file_2, FileStorage::WRITE);
  int i = 1, j = 2;
  cout << "Calculating the related homography matrices...\n";
  for(auto &element: h1)
  {
    tmp = tmp * element;
    h2.push_back(tmp);
    string H_mat_name = "H" + to_string(i) + to_string(j);
    //cout << H_mat_name << endl;
    fs << H_mat_name << tmp;
    j ++;
  }
  fs.release();
  cout << "Finished calculating realated homography matrices.\n";
}


/*Function that wraps the images
Usage:
Parameters:
vector<string> v: a vector of strinngs that holds the list of images to be wrapped
vector<Mat> out_img_vec: a vector of matrices that has the wrapped images
Returns:
returns nothing, all parameters are passed by reference
*/
void wrap_images(stringvec& v, vector<Mat> &out_img_vec)
{
  vector<string>::const_iterator img_name = v.begin();
  Mat first_img = imread(*img_name, CV_LOAD_IMAGE_UNCHANGED);
  Mat tmp = Mat::eye(3,3,CV_64FC1);
  tmp.at<double>(1,2) += 800;
  Mat tmp_1;
  cv:Size s2;
  //set size of the warped image
  s2.width = 6 * first_img.rows;
  s2.height  = 2 * first_img.cols;
  warpPerspective (first_img, tmp_1, tmp, s2);
  /*namedWindow("Original Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
  resizeWindow("Original Image", 1000, 600);
  namedWindow("Warped Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
  resizeWindow("Warped Image", 1000, 600);
  moveWindow("Warped Image", 1100,0);
  imshow("Original Image", first_img);
  imshow( "Warped Image", tmp_1);
  waitKey(0);*/
  out_img_vec.push_back(tmp_1);
  img_name ++;
  //creating and opening a xml file to store homography matrix values realted to the first image
  string homography_mat_file_2 = "related_homography_matrices.xml";
  FileStorage fs(homography_mat_file_2, FileStorage::READ);
  cout << "Starting to warp the images...\n";
  int i = 1, j = 2;
  for(; img_name != v.end(); img_name++)
  {
    Mat in_img, out_img, H;
    cout << "Warping image: " << *img_name << endl;
    in_img = imread(*img_name, CV_LOAD_IMAGE_UNCHANGED);
    string H_mat_name = "H" + to_string(i) + to_string(j);
    fs[H_mat_name] >> H;
    H.at<double>(1,2) += 800;
    //cout << "The value of H is:" << endl << H << endl;
    Size s;
    //setting the size of the warped image
    s.width = 6 * first_img.rows;
    s.height  = 2 * first_img.cols;
    warpPerspective (in_img, out_img, H, s);
    out_img_vec.push_back(out_img);
    namedWindow("Original Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
    resizeWindow("Original Image", 1000, 600);
    namedWindow("Warped Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
    resizeWindow("Warped Image", 1000, 600);
    moveWindow("Warped Image", 1100,0);
    imshow("Original Image", in_img);
    imshow( "Warped Image", out_img);
    string warp_name = "warped_images/wrap" + to_string(i) + to_string(j) + ".jpg";
    imwrite(warp_name, out_img);
    waitKey(1000);
    destroyAllWindows();
    j++;
  }
  cout << "Warping done.\n";
}


/*Function that computes the pairwise homographies
Usage:
Parameters:
vector<string> v: a vector of strings that has the names of the images
Returns:
returns nothing
*/
void calc_homographies(stringvec& v)
{

  if(v.size() <2)
  {
    cout << "Folder empty";
    return;
  }
  vector<string>::const_iterator i = v.begin();
  vector<string>::const_iterator j = i + 1;
  int k = 1, l = 2;
  //creating and opening a xml file to store homography matrix values
  string homography_mat_file = "homography_matrices.xml";
  FileStorage fs(homography_mat_file, FileStorage::WRITE);
  vector<Mat> H_list, H_related_list;
  for (; j != v.end() ; j++, i++, k++, l++)
  {
    Mat img_object = imread( *i, CV_LOAD_IMAGE_GRAYSCALE );
    Mat img_scene = imread( *j, CV_LOAD_IMAGE_GRAYSCALE );
    //checking if both the images are readable
    if( !img_object.data || !img_scene.data )
    {
      std::cout<< " --(!) Error reading images " << std::endl;
      return;
    }

    //-- Step 1: Detect the keypoints and extract descriptors using SURF
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create( minHessian );
    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    detector->detectAndCompute( img_object, Mat(), keypoints_object, descriptors_object );
    detector->detectAndCompute( img_scene, Mat(), keypoints_scene, descriptors_scene );
    //-- Step 2: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );
    double max_dist = 0; double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      double dist = matches[i].distance;
      if( dist < min_dist ) min_dist = dist;
      if( dist > max_dist ) max_dist = dist;
    }
    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;
    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      if( matches[i].distance <= 3*min_dist )
       {
         good_matches.push_back( matches[i]);
       }
    }
    Mat img_matches;
    drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    for( size_t i = 0; i < good_matches.size(); i++ )
    {
      //-- Get the keypoints from the good matches
      obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
      scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }
    Mat H = findHomography(scene, obj, RANSAC );
    H_list.push_back(H);
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
    std::vector<Point2f> scene_corners(4);
    perspectiveTransform( obj_corners, scene_corners, H);
    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    //-- Show detected matches
    namedWindow("Good Matches & Object detection", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
    resizeWindow("Good Matches & Object detection", 2200, 1000);
    imshow( "Good Matches & Object detection", img_matches );
    waitKey(1000);
    destroyAllWindows();

    string H_mat_name = "H" + to_string(k) + to_string(l);
    //cout << H_mat_name << endl;
    fs << H_mat_name << H;

  }
  fs.release();

  relate_homography(H_list, H_related_list);
  vector<Mat> imgs;
  Mat super_image;
  wrap_images(v, imgs);
  stitch_images(super_image, imgs);
  //rotating the super image about the optic center by 45 degrees
  rotate_super_image(super_image);
}

/*Function that stitches the wrapped images
Usage:
Parameters:
Mat super_image: matrix that stores the stitched super image
vector<Mat> imgs: a vector of matrices that has the images that are to be stitched
Returns:
returns nothing, all parameters are passed by reference
*/
void stitch_images(Mat& super_image, vector<Mat>& imgs)
{
  vector<Mat>::const_iterator img_itr = imgs.begin();
  super_image = *img_itr;
  int l = 0;
  for(auto &element : imgs)
  {
    cout << "Stitching image " + to_string(l + 1) + " and image " + to_string(l+2) + "..." << endl;
    for(int i=0; i<element.rows; i++)
    {
        for(int j=0; j<element.cols; j++)
        {
          Vec3b tmp = Vec3b(0, 0, 0);
          //cout << "element rows: " << element.rows << " element cols: " << element.cols << endl;
          //cout << "super image rows: " << super_image.rows << " super image cols: " << super_image.cols << endl;
          if((element.at<cv::Vec3b>(i,j) == tmp) && (super_image.at<cv::Vec3b>(i,j) == tmp))
          {
            for (int k = 0; k < 3; k++)
            {
              //super_image.at<cv::Vec3b>(i,j)[k] = (super_image.at<cv::Vec3b>(i,j)[k] + element.at<cv::Vec3b>(i,j)[k]) / 2.0;
              super_image.at<cv::Vec3b>(i,j)[k] = 0;
            }
          }
          else
          {
            if((element.at<cv::Vec3b>(i,j) != tmp) && (super_image.at<cv::Vec3b>(i,j) == tmp))
            {
              for (int k = 0; k < 3; k++)
              {
                //super_image.at<cv::Vec3b>(i,j)[k] = (super_image.at<cv::Vec3b>(i,j)[k] + element.at<cv::Vec3b>(i,j)[k]) / 2.0;
                super_image.at<cv::Vec3b>(i,j)[k] = element.at<cv::Vec3b>(i,j)[k];
              }
            }
            else
            {
              if((element.at<cv::Vec3b>(i,j) != tmp) && (super_image.at<cv::Vec3b>(i,j) != tmp))
              {
                for (int k = 0; k < 3; k++)
                {
                  super_image.at<cv::Vec3b>(i,j)[k] = (super_image.at<cv::Vec3b>(i,j)[k] + element.at<cv::Vec3b>(i,j)[k]) / 2.0;
                }
              }
            }
          }
        }
    }
    l++;
  }
  namedWindow("Super Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
  resizeWindow("Super Image", 9000, 1000);
  imshow( "Super Image", super_image );
  imwrite("super_image_1.jpg", super_image);
  waitKey(0);
  destroyAllWindows();
}

/*Function that rotates the super image
Usage:
Parameters:
Mat super_image: Matrix that stores the super image
Returns:
returns nothing, the matrix that is used to store the rotated image is passed by reference
*/
void rotate_super_image(Mat& super_image)
{
  //Initializing a matrices to store the intrinsic camera camera matrix and the homography matrix
  Mat K, H, rotated_super_image;
  //Initializing a Mat object to store the rotation matrix
  Mat R = Mat::zeros(3,3,CV_64FC1);
  //Setting the rotation matrix values
  R.at<double>(0,0) = 0.7071;
  R.at<double>(0,2) = -0.7071;
  R.at<double>(1,1) = 1;
  R.at<double>(2,0) = 0.7071;
  R.at<double>(2,2) = 0.7071;
  //Getting the value of the intrinsic camera matrix
  FileStorage fs("camera_params.xml", FileStorage::READ);
  fs["camera_matrix"] >> K;
  fs.release();
  //Calulating the homography to rotate the image
  H = K * R * K.inv();
  //Padding the y translation so that the resulting image falls in the image frame
  H.at<double>(0,2) += 3500;
  //H.at<double>(1,2) += 200;
  //Rotating the super image
  cv:Size s;
  s.width = super_image.cols / 3 ;
  s.height  =  1.3 * super_image.rows;
  warpPerspective (super_image, rotated_super_image, H, s);
  namedWindow("Rotated Super Image", CV_WINDOW_FREERATIO | CV_GUI_NORMAL);
  resizeWindow("Rotated Super Image", 9000, 1000);
  imshow( "Rotated Super Image", rotated_super_image );
  imwrite("rotated_super_image_1.jpg", rotated_super_image);
  waitKey(0);
  destroyAllWindows();

}
