#ifndef GENSLITSCAN_H
#define GENSLITSCAN_H

#include <iostream>
#include <cmath>
#include <string>
#include <exception>
#include <opencv2/opencv.hpp>

#include "../../importer/eye-tracker-output/include/tobii_importer.h"

class SlitScanVideoError: public std::exception
{
  virtual const char* what() const throw()
  {
    return "genslitscan ::: at reading frame";
  }
};


struct VideoInfo {
    double fpms;
    int frameCount;
    int fourCC;
    int frameWidth;
    int frameHeight;
};

class SlitscanGenerator {
public:
    cv::Mat createVisualization(cv::VideoCapture &inputVideo, const std::vector<struct TobiiExportData> &pData);
    cv::Mat createVisualization(const std::string &inputVideoPath, const std::vector<struct TobiiExportData> &pData);

    cv::Mat getVC123(void);
    cv::Mat getVC23(void);
    cv::Mat getVC1(void);
    cv::Mat getVC2(void);
    cv::Mat getVC3(void);

    static VideoInfo readVideoInfo(const std::string &inputVideoPath);
private:
    cv::Scalar ScalarHLS2BGR(uchar H, uchar L, uchar S);
    static bool isValid(const TobiiExportData &data);
    cv::Mat createSlitScanColumn(const cv::Mat &frame, int centerX, unsigned int avgWindowRadius = 0);
    // Spectrogram
    cv::Mat vc1;
    // Slit-Scan
    cv::Mat vc2;
    // Focus Map
    cv::Mat vc3;
    // Slit-Scan + Focus-Map
    cv::Mat vc23;
    // Spectrogram + Slit-Scan + Focus-Map
    cv::Mat vc123;
};



#endif // GENSLITSCAN_H
