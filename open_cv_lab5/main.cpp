#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::vector;

static int top{118}, bot{444}, height{67}, b{220};
const int maxHeight{540}, maxBot{600}, maxTop{470};

void findLines(const Mat &bin, vector<Point2f> &centers) {
  Rect window(0, 0, 20, 20);
  vector<Point2f> nonZeros;

  for (window.y = 0; window.y < bin.rows; window.y += 20) {
    for (window.x = 0; window.x + 10 < bin.cols; window.x += 10) {
      Mat clice = bin(window);
      vector<Point2f> roi;

      findNonZero(clice, roi);
      if (!roi.empty()) {
        for (unsigned i = 0; i < roi.size(); ++i) roi[i].x += window.x;
        nonZeros.insert(nonZeros.end(), roi.begin(), roi.end());
      } else if (!nonZeros.empty()) {
        centers.push_back(
            Point((nonZeros.front().x + nonZeros.back().x) / 2,
                  window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
        nonZeros.clear();
      }
    }
    if (!nonZeros.empty()) {
      centers.push_back(
          Point((nonZeros.front().x + nonZeros.back().x) / 2,
                window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
      nonZeros.clear();
    }
  }
}

void warp(Mat &src, Mat &dst) {
  Point2f src_vertices[4];
  src_vertices[0] = Point(650 - top, maxHeight - height);
  src_vertices[1] = Point(650 + top, maxHeight - height);
  src_vertices[2] = Point(700 + bot, src.rows - 100);
  src_vertices[3] = Point(700 - bot, src.rows - 100);

  Point2f dst_vertices[4];
  dst_vertices[0] = Point(0, 0);
  dst_vertices[1] = Point(dst.cols, 0);
  dst_vertices[2] = Point(dst.cols, dst.rows);
  dst_vertices[3] = Point(0, dst.rows);

  Mat M = getPerspectiveTransform(src_vertices, dst_vertices);
  warpPerspective(src, dst, M, dst.size());

  line(src, Point(650 - top, maxHeight - height),
       Point(650 + top, maxHeight - height), Scalar(0, 255, 0), 1);
  line(src, Point(650 + top, maxHeight - height),
       Point(700 + bot, src.rows - 100), Scalar(0, 255, 0), 1);
  line(src, Point(700 + bot, src.rows - 100), Point(700 - bot, src.rows - 100),
       Scalar(0, 255, 0), 1);
  line(src, Point(700 - bot, src.rows - 100),
       Point(650 - top, maxHeight - height), Scalar(0, 255, 0), 1);

  Mat bin;
  dst.copyTo(bin);

  vector<Point2f> centers;
  threshold(bin, bin, b, 255, THRESH_BINARY);
  cvtColor(bin, bin, COLOR_BGR2GRAY);
  findLines(bin, centers);

  createTrackbar("Top", "bin", &b, 255);
  resize(bin, bin, Size(), 0.5, 0.5);
  imshow("bin", bin);

  Mat reverseM = getPerspectiveTransform(dst_vertices, src_vertices);
  perspectiveTransform(centers, centers, reverseM);

  for (auto it : centers) {
    circle(src, it, 3, Scalar(0, 255, 0), FILLED);
  }
}

int main() {
  VideoCapture cap("challenge.mp4");

  namedWindow("source");
  namedWindow("bin");

  createTrackbar("Top", "source", &top, maxTop);
  createTrackbar("Bottom", "source", &bot, maxBot);
  createTrackbar("Height", "source", &height, maxHeight);

  while (true) {
    auto begin = std::chrono::steady_clock::now();
    Mat src;
    cap >> src;

    if (src.empty()) {
      fprintf(stdout, "End of video\n");
      break;
    }

    Mat dst;
    src.copyTo(dst);
    warp(src, dst);

    // time for FPS
    auto end = std::chrono::steady_clock::now();
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    resize(src, src, Size(), 0.5, 0.5);
    resize(dst, dst, Size(), 0.5, 0.5);
    imshow("source", src);
    imshow("result", dst);

    if (waitKey(/*1000 / 25 - ms.count()*/ 70) == 27) break;
  }

  cap.release();
  return 0;
}
