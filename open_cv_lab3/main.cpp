#include <opencv2/opencv.hpp>

using namespace cv;

const int max_value_H = 360;
const int max_value = 255;
const int max_E = 500;
static int low_H = 0, low_S = 0, low_V = 0, low_E = 0;
static int high_H = max_value_H, high_S = max_value, high_V = max_value,
           high_E = max_E;

static void on_low_H_thresh_trackbar(int, void *) {
  low_H = min(high_H - 1, low_H);
  setTrackbarPos("Low H", "settings", low_H);
}
static void on_high_H_thresh_trackbar(int, void *) {
  high_H = max(high_H, low_H + 1);
  setTrackbarPos("High H", "settings", high_H);
}
static void on_low_S_thresh_trackbar(int, void *) {
  low_S = min(high_S - 1, low_S);
  setTrackbarPos("Low S", "settings", low_S);
}
static void on_high_S_thresh_trackbar(int, void *) {
  high_S = max(high_S, low_S + 1);
  setTrackbarPos("High S", "settings", high_S);
}
static void on_low_V_thresh_trackbar(int, void *) {
  low_V = min(high_V - 1, low_V);
  setTrackbarPos("Low V", "settings", low_V);
}
static void on_high_V_thresh_trackbar(int, void *) {
  high_V = max(high_V, low_V + 1);
  setTrackbarPos("High V", "settings", high_V);
}
static void on_low_E_thresh_trackbar(int, void *) {
  low_E = min(high_E - 1, low_E);
  setTrackbarPos("Low E", "edges settings", low_E);
}
static void on_high_E_thresh_trackbar(int, void *) {
  high_E = max(high_E, low_E + 1);
  setTrackbarPos("High E", "edges settings", high_E);
}

int main() {
  VideoCapture cap(0);
  namedWindow("settings");
  namedWindow("edges settings");

  createTrackbar("Low H", "settings", &low_H, max_value_H,
                 on_low_H_thresh_trackbar);
  createTrackbar("High H", "settings", &high_H, max_value_H,
                 on_high_H_thresh_trackbar);
  createTrackbar("Low S", "settings", &low_S, max_value,
                 on_low_S_thresh_trackbar);
  createTrackbar("High S", "settings", &high_S, max_value,
                 on_high_S_thresh_trackbar);
  createTrackbar("Low V", "settings", &low_V, max_value,
                 on_low_V_thresh_trackbar);
  createTrackbar("High V", "settings", &high_V, max_value,
                 on_high_V_thresh_trackbar);

  createTrackbar("Low E", "edges settings", &low_E, max_E,
                 on_low_E_thresh_trackbar);
  createTrackbar("High E", "edges settings", &high_E, max_E,
                 on_high_E_thresh_trackbar);

  while (true) {
    Mat frame, bluredFrame, blueFrames, edges;
    cap >> frame;

    if (frame.empty()) break;
    imshow("this is you, smile! :)", frame);

    blur(frame, bluredFrame, Size(7, 7));
    imshow("this is blurry you, smile! :)", bluredFrame);

    // Scalars for green color: (26, 70, 33), (76, 108, 71)
    cvtColor(frame, blueFrames, COLOR_BGR2HSV);
    inRange(frame, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V),
            blueFrames);
    imshow("blue objects", blueFrames);

    // edges : 85, 112
    cvtColor(frame, edges, COLOR_BGR2GRAY);
    Canny(frame, edges, low_E, high_E);
    imshow("edges", edges);

    if (waitKey(10) == 27) break;  // ESC
  }

  cap.release();
  return 0;
}
