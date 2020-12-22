#include <chrono>
#include <opencv2/opencv.hpp>

using namespace cv;

static int top{118}, bot{444}, height{67};
const int maxHeight{540}, maxBot{600}, maxTop{470};

void warp(const Mat &src, Mat &dst) {
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
}

int main() {
  VideoCapture cap("challenge.mp4");

  namedWindow("source");

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

    if (waitKey(1000 / 25 - ms.count()) == 27) break;
  }

  cap.release();
  return 0;
}
