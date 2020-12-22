#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::vector;

void customSmoothing(const Mat& src, Mat& result, unsigned size);
void customGradient(const Mat& src, Mat& result);
void customFilter(const Mat& src, Mat& result,
                  const vector<vector<double> >& kernel);
uchar calculatePixel(const Mat& src, const vector<vector<double> >& kernel,
                     int i, int j);

int main(int argc, char** argv) {
  if (argc != 2) return -1;

  Mat image, imageRes;
  image = imread(argv[1], 1);

  GaussianBlur(image, imageRes, Size(15, 15), 0, 0);
  namedWindow("Pussy Gaussian", WINDOW_AUTOSIZE);
  imshow("Pussy Gaussian", imageRes);

  customSmoothing(image, imageRes, 5);
  namedWindow("Pussy Custom", WINDOW_AUTOSIZE);
  imshow("Pussy Custom", imageRes);

  cvtColor(image, image, COLOR_BGR2GRAY);
  customGradient(image, imageRes);
  namedWindow("Pussy Gradient", WINDOW_AUTOSIZE);
  imshow("Pussy Gradient", imageRes);

  waitKey(0);
  return 0;
}

void customSmoothing(const Mat& src, Mat& result, unsigned size) {
  src.copyTo(result);

  vector<vector<double> > kernel(size,
                                 vector<double>(size, 1. / (size * size)));

  customFilter(src, result, kernel);
}

void customGradient(const Mat& src, Mat& result) {
  src.copyTo(result);

  vector<vector<double> > kernel = {{3, 10, 3}, {0, 0, 0}, {-3, -10, -3}};

  customFilter(src, result, kernel);
}

void customFilter(const Mat& src, Mat& result,
                  const vector<vector<double> >& kernel) {
  int rows = src.rows;
  int border = static_cast<int>(kernel.size() / 2);
  if (src.type() == 0) {
    for (int i = border; i < rows - border + 1; ++i) {
      for (int j = border; j < rows - border + 1; ++j) {
        result.at<uchar>(i, j) = calculatePixel(src, kernel, i, j);
      }
    }
  } else {
    vector<Mat> channels;
    split(src, channels);
    vector<Mat> results = channels;

    for (int i = border; i < rows - border + 1; ++i) {
      for (int j = border; j < rows - border + 1; ++j) {
        results[0].at<uchar>(i, j) = calculatePixel(channels[0], kernel, i, j);
        results[1].at<uchar>(i, j) = calculatePixel(channels[1], kernel, i, j);
        results[2].at<uchar>(i, j) = calculatePixel(channels[2], kernel, i, j);
      }
    }

    merge(results, result);
  }
}

uchar calculatePixel(const Mat& src, const vector<vector<double> >& kernel,
                     int i, int j) {
  int newPixel = 0;
  int border = static_cast<int>(kernel.size() / 2);

  for (unsigned k = 0; k < kernel.size(); ++k) {
    for (unsigned l = 0; l < kernel.size(); ++l) {
      newPixel += src.at<uchar>(i + static_cast<int>(k) - border,
                                j + static_cast<int>(l) - border) *
                  kernel[k][l];
    }
  }

  return saturate_cast<uchar>(newPixel);
}
