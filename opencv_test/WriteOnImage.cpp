//Opencv c++ code for Overlaying a Text on an Image
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;
  image=imread(argv[1], 1);

  if(!image.data){
    printf("Error loading image \n");
    return -1;
  }
  cout << "Width : " << image.cols << endl;
  cout << "Height: " << image.rows << endl;

  putText(image, "Maximum elevation (m)", Point(0, image.rows-100), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, true);
  putText(image, "Test123 (m)", Point(0, image.rows-65), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, true);
  putText(image, "Test456 (m)", Point(0, image.rows-30), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, true);

  putText(image, "adapt-lab", Point(image.cols-200, image.rows-20), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2, true);

  //resize(image2, image2, Size(image2.cols/15, image2.rows/15));

  try{
    imwrite("new_heatmap.png", image);
  }
  catch (runtime_error& ex){
    fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
    return 1;
  }

  fprintf(stdout, "Saved PNG file with file name new_heatmap.png.\n");
  return 0;
}
