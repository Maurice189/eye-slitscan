#ifndef HISTO_MEASURE_H
#define HISTO_MEASURE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "QtOpenCV/cvmatandqimage.h"

#include <stdexcept>

struct ImageData {
    void* data;
    int width;
    int height;
};



enum class ColorSpace {RGB, HSV, CIE_LAB};
enum class HistogramMetric {CORRELATION, CHI_SQUARE, INTERSECTION, BHATTACHARYYA, EMD};


cv::Mat calcSignature(cv::MatND &hist, ColorSpace cs);

//double histogramScore(const QImage &img01, const QImage &img02, HistogramMetric metric = HistogramMetric::BHATTACHARYYA, ColorSpace histogramColorSpace = ColorSpace::HSV);
double histogramScore(const cv::Mat &img01, const cv::Mat &img02, HistogramMetric metric = HistogramMetric::BHATTACHARYYA, ColorSpace histogramColorSpace = ColorSpace::HSV);
double compareHistogram(const cv::MatND &imgHist01, const cv::MatND &imgHist02, HistogramMetric metric);
void calcHistogram(const cv::Mat &inMatImg, cv::MatND &outHist, ColorSpace histogramColorSpace);

#endif // HISTO_MEASURE_H
