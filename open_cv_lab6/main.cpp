#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::vector;

static int top{118}, bot{444}, height{67}, b{220};
const int maxHeight{540}, maxBot{600}, maxTop{470};

vector<double> approx(vector<Point2f> p) {
  int pts = p.size();
  double sums[pts][pts], b[pts];
  vector<double> a(pts);
  int K = 2;
  double x[pts];
  double y[pts];
  for (int i = 0; i < pts; ++i) {
    x[i] = p[i].y;
    y[i] = p[i].x;
  }

  int i, j, k;
  long double s, t, M;

  for (i = 0; i < pts; i++) {
    for (int j = i; j >= 1; j--)
      if (x[j] < x[j - 1]) {
        t = x[j - 1];
        x[j - 1] = x[j];
        x[j] = t;
        t = y[j - 1];
        y[j - 1] = y[j];
        y[j] = t;
      }
  }

  for (i = 0; i < K + 1; i++) {
    for (j = 0; j < K + 1; j++) {
      sums[i][j] = 0;
      for (k = 0; k < pts; k++) sums[i][j] += pow(x[k], i + j);
    }
  }

  for (i = 0; i < K + 1; i++) {
    b[i] = 0;
    for (k = 0; k < pts; k++) b[i] += pow(x[k], i) * y[k];
  }

  for (k = 0; k < K + 1; k++) {
    for (i = k + 1; i < K + 1; i++) {
      M = sums[i][k] / sums[k][k];
      for (j = k; j < K + 1; j++) sums[i][j] -= M * sums[k][j];
      b[i] -= M * b[k];
    }
  }

  for (i = K; i >= 0; i--) {
    s = 0;
    for (j = i; j < K + 1; j++) s += sums[i][j] * a[j];
    a[i] = (b[i] - s) / sums[i][i];
  }

  return a;
}

void findLines(const Mat &bin, vector<vector<Point2f> > &centers) {
  Rect window(0, 0, 40, 40);
  vector<Point2f> nonZeros;

  for (window.y = 0; window.y + 40 <= bin.rows; window.y += 40) {
    for (window.x = 0; window.x + 20 < bin.cols; window.x += 20) {
      Mat clice = bin(window);
      vector<Point2f> roi;

      findNonZero(clice, roi);
      if (!roi.empty()) {
        for (unsigned i = 0; i < roi.size(); ++i) roi[i].x += window.x;
        nonZeros.insert(nonZeros.end(), roi.begin(), roi.end());
      } else if (!nonZeros.empty()) {
        if (window.x < bin.cols / 2)
          centers[0].push_back(
              Point((nonZeros.front().x + nonZeros.back().x) / 2,
                    window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
        else
          centers[1].push_back(
              Point((nonZeros.front().x + nonZeros.back().x) / 2,
                    window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
        nonZeros.clear();
      }
    }
    if (!nonZeros.empty()) {
      centers[1].push_back(
          Point((nonZeros.front().x + nonZeros.back().x) / 2,
                window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
      nonZeros.clear();
    }
  }
}

void warp(Mat &src, Mat &dst) {
  Mat poly;
  src.copyTo(poly);

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

  vector<vector<Point2f> > centers(2);
  threshold(bin, bin, b, 255, THRESH_BINARY);
  cvtColor(bin, bin, COLOR_BGR2GRAY);
  findLines(bin, centers);

  Mat reverseM = getPerspectiveTransform(dst_vertices, src_vertices);
  perspectiveTransform(centers[0], centers[0], reverseM);
  perspectiveTransform(centers[1], centers[1], reverseM);

  vector<Point> leftPoints;
  vector<Point> rightPoints;
  vector<double> leftPolynom = approx(centers[0]);
  vector<double> rightPolynom = approx(centers[1]);
  for (auto y = centers[0][0].y; y < centers[0].back().y; y += 0.1) {
    double x = 0;
    for (unsigned i = 0; i < leftPolynom.size(); ++i) {
      x += leftPolynom[i] * pow(y, i);
    }
    leftPoints.push_back(Point(x, y));

    x = 0;
    for (unsigned i = 0; i < rightPolynom.size(); ++i) {
      x += rightPolynom[i] * pow(y, i);
    }
    rightPoints.push_back(Point(x, y));
  }

  vector<vector<Point> > points;
  vector<Point> p;
  points.push_back(leftPoints);
  points[0].insert(points[0].end(), rightPoints.rbegin(), rightPoints.rend());
  if (!points[0].empty()) fillPoly(poly, points, Scalar(255, 0, 0));
  resize(poly, poly, Size(), 0.5, 0.5);
  imshow("road", poly);

  for (auto it : centers[0]) {
    circle(src, it, 3, Scalar(0, 255, 0), FILLED);
  }

  for (auto it : centers[1]) {
    circle(src, it, 3, Scalar(0, 255, 0), FILLED);
  }
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

    if (waitKey(/*1000 / 25 - ms.count()*/ 30) == 27) break;
  }

  cap.release();
  return 0;
}
