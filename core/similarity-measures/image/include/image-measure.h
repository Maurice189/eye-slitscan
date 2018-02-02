#ifndef IMAGE_MEASURE_H
#define IMAGE_MEASURE_H

#include <iostream>
#include <exception>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


enum class ImageMetric {SQUARED_DIFFERENCES, CROSS_CORRELATION, COSINE_COEFFICIENT};

class IncompatibleImageDimensions: public std::exception
{
  virtual const char* what() const throw()
  {
    return "genslitscan ::: at reading frame";
  }
};


double imageScore(const cv::Mat &imgMat1, const cv::Mat &imgMat2, ImageMetric metric);

#endif // IMAGE_MEASURE_H
