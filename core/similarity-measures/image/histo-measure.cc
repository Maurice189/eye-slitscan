#include "histo-measure.h"
#include <QImage>

double histogramScore(const cv::Mat &img01, const cv::Mat &img02, HistogramMetric metric, ColorSpace histogramColorSpace)
{
    cv::MatND imgHist01, imgHist02;
    // Calculate histograms for the two images
    calcHistogram(img01, imgHist01, histogramColorSpace);
    calcHistogram(img02, imgHist02, histogramColorSpace);

    // Normalize hist between [0,1]
    cv::normalize(imgHist01, imgHist01, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(imgHist02, imgHist02, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // EMD has to be treated specially, since we need signatures here - See OpenCV Doc. !
    if (metric == HistogramMetric::EMD) {
         auto sigHist01 = calcSignature(imgHist01, histogramColorSpace);
         auto sigHist02 = calcSignature(imgHist02, histogramColorSpace);

         return cv::EMD(sigHist01, sigHist02, CV_DIST_L2);
    }

    return compareHistogram(imgHist01, imgHist02, metric);
}


cv::Mat calcSignature(cv::MatND &hist, ColorSpace cs)
{
    // As described in the OpenCV - Docs
    // Every entry starts with its bin value.
    // Afterwards the position is appended.

    if (cs == ColorSpace::RGB) {
        int histSize[] = {30, 30, 30};
        cv::Mat sig(histSize[0] * histSize[1] * histSize[2], 4, CV_32FC1);
        for(int r = 0; r < histSize[0]; r++) {
            for(int g = 0; g < histSize[1]; ++g) {
                for(int b = 0; b < histSize[2]; ++b) {
                    float binVal = hist.at<float>(r,g,b);
                    sig.at<float>(r*histSize[1]*histSize[2] + g*histSize[2] + b, 0) = binVal;
                    sig.at<float>(r*histSize[1]*histSize[2] + g*histSize[2] + b, 1) = r;
                    sig.at<float>(r*histSize[1]*histSize[2] + g*histSize[2] + b, 2) = g;
                    sig.at<float>(r*histSize[1]*histSize[2] + g*histSize[2] + b, 3) = b;
                }
            }
        }
        return sig;
    } else if (cs == ColorSpace::HSV) {
        int histSize[] = {30, 30};
        cv::Mat sig(histSize[0] * histSize[1], 3, CV_32FC1);
        for(int h = 0; h < histSize[0]; h++) {
            for(int s = 0; s < histSize[1]; ++s) {
                float binVal = hist.at<float>(h,s);
                sig.at<float>(h*histSize[1] + s, 0) = binVal;
                sig.at<float>(h*histSize[1] + s, 1) = h;
                sig.at<float>(h*histSize[1] + s, 2) = s;
            }
        }
        return sig;
    } else if (cs == ColorSpace::CIE_LAB) {
        int histSize[] = {30, 30};
        cv::Mat sig(histSize[0] * histSize[1], 3, CV_32FC1);
        for(int a = 0; a < histSize[0]; a++) {
            for(int b = 0; b < histSize[1]; ++b) {
                float binVal = hist.at<float>(a,b);
                sig.at<float>(a*histSize[1] + b, 0) = binVal;
                sig.at<float>(a*histSize[1] + b, 1) = a;
                sig.at<float>(a*histSize[1] + b, 2) = b;
            }
        }
        return sig;
    }
}

/*
double histogramScore(const QImage &img01, const QImage &img02, HistogramMetric metric, ColorSpace histogramColorSpace)
{
    
    if (img01.isNull()) {
        throw std::invalid_argument("histogramScore ::: first image parameter is null");
    }
    if (img02.isNull()) {
        throw std::invalid_argument("histogramScore ::: second image parameter is null");
    }

    cv::Mat imgMat01 = QtOcv::image2Mat_shared(img01);
    cv::Mat imgMat02 = QtOcv::image2Mat_shared(img02);

    cv::MatND imgHist01, imgHist02;
    // Calculate histograms for the two images
    calcHistogram(imgMat01, imgHist01, histogramColorSpace);
    calcHistogram(imgMat02, imgHist02, histogramColorSpace);

    return compareHistogram(imgHist01, imgHist02, metric);

}*/

double compareHistogram(const cv::MatND &imgHist01, const cv::MatND &imgHist02, HistogramMetric metric)
{
    int method;

    switch(metric) {
        case HistogramMetric::CORRELATION: 
            method = CV_COMP_CORREL;
            break;
        case HistogramMetric::CHI_SQUARE: 
            method = CV_COMP_CHISQR;
            break;
        case HistogramMetric::INTERSECTION: 
            method = CV_COMP_INTERSECT;
            break;
        case HistogramMetric::BHATTACHARYYA: 
            method = CV_COMP_BHATTACHARYYA;
            break;
        default:
            method = CV_COMP_BHATTACHARYYA;
    }

    return cv::compareHist(imgHist01, imgHist02, method);
}
void calcHistogram(const cv::Mat &inMatImg, cv::MatND &outHist, ColorSpace histogramColorSpace)
{
    cv::Mat bgr;
    cv::cvtColor(inMatImg, bgr, CV_BGRA2BGR);

    switch(histogramColorSpace) {
         case ColorSpace::HSV:
            {
                int histSize[] = {30, 30};
                cv::Mat hsv;
                cv::cvtColor(bgr, hsv, CV_BGR2HSV);

                //Quantize the hue to 30 levels
                // and the saturation to 32 levels
                // hue varies from 0 to 179, see cvtColor
                float hranges[] = { 0, 180 };
                // saturation varies from 0 (black-gray-white) to
                // 255 (pure spectrum color)
                float sranges[] = { 0, 256 };
                const float* ranges[] = { hranges, sranges };
                // we compute the histogram from the 0-th and 1-st channels
                int channels[] = {0, 1};

                return calcHist( &hsv, 1, channels, cv::Mat(), // do not use mask
                    outHist, 2, histSize, ranges,
                    true, // the histogram is uniform
                    false );
            }
            break;
         case ColorSpace::CIE_LAB:
            {
                int histSize[] = {30, 30};
                cv::Mat lab;
                cv::cvtColor(bgr, lab, CV_BGR2Lab);
                // AB - Varies from 0 to 255 (for 8Bit - Images)
                float aRanges[] = { 0, 256 };
                float bRanges[] = { 0, 256 };

                const float* ranges[] = { aRanges, bRanges };
                int channels[] = {1, 2};

                return calcHist( &lab, 1, channels, cv::Mat(), // do not use mask
                    outHist, 2, histSize, ranges,
                    true, // the histogram is uniform
                    false );
            }
            break;
         case ColorSpace::RGB:
            // We are already in RGB, so nothing to do.
            {
                int histSize[] = {30, 30, 30};
                cv::Mat rgb;
                cv::cvtColor(bgr, rgb, CV_BGR2RGB);
                // RGB - Varies from 0 to 255
                float Rranges[] = { 0, 256 };
                float Granges[] = { 0, 256 };
                float Branges[] = { 0, 256 };

                const float* ranges[] = { Rranges, Granges , Branges};
                int channels[] = {0, 1, 2};

                return calcHist( &bgr, 1, channels, cv::Mat(), // do not use mask
                    outHist, 3, histSize, ranges,
                    true, // the histogram is uniform
                    false );
            }
            break;
         default:
            assert(false);
    }
}
