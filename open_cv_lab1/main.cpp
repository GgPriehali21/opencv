#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) return -1;

  Mat imageOrig, imageGrey, imageHsv, imageBin, imageSegm;
  imageOrig = imread(argv[1], 1);
  imageGrey = imread(argv[1], 0);

  cvtColor(imageOrig, imageHsv, COLOR_BGR2HSV);
  threshold(imageGrey, imageBin, 128, 255, THRESH_BINARY);

  namedWindow("Pussy Original", WINDOW_AUTOSIZE);
  imshow("Pussy Original", imageOrig);

  namedWindow("Pussy Grey", WINDOW_AUTOSIZE);
  imshow("Pussy Grey", imageGrey);

  namedWindow("Pussy HSV", WINDOW_AUTOSIZE);
  imshow("Pussy HSV", imageHsv);

  namedWindow("Pussy Bin", WINDOW_AUTOSIZE);
  imshow("Pussy Bin", imageBin);

  ellipse(imageOrig, RotatedRect(Point2f(130, 140), Size2f(120, 80), 15),
          Scalar(0, 0, 255), 3);

  namedWindow("Pussy with ellipse", WINDOW_AUTOSIZE);
  imshow("Pussy with ellipse", imageOrig);

  waitKey(0);
  return 0;
}
