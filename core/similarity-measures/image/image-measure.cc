#include "image-measure.h"

double imageScore(const cv::Mat &imgMat1, const cv::Mat &imgMat2, ImageMetric metric)
{
    cv::Mat result;
    cv::Mat mat01, mat02;

    /*
     * Most of the time the slitscans of two participants differ
     * in the number of columns, since they usually don't have the same
     * exact number of sampled gaze points.
     *
     * In those cases we have to take care about defining the correct template, because
     * the template might not be bigger than the image.
     *
     */

    if (imgMat1.cols <= imgMat2.cols && imgMat1.rows <= imgMat2.rows) {
        mat01 = imgMat2;
        mat02 = imgMat1;    
    } else if (imgMat2.cols <= imgMat1.cols && imgMat2.rows <= imgMat1.rows) {
        mat01 = imgMat1;
        mat02 = imgMat2;
    } else {
        // This will hopfully never happen because the slitscans have never different heights
        // since the height is determined by the frame height, which is fixed for all slitscans
        // from one source
        throw IncompatibleImageDimensions();
    }

    cv::cvtColor(mat01, mat01, CV_BGR2HSV);
    cv::cvtColor(mat02, mat02, CV_BGR2HSV);

    int method;

    switch (metric) {
    case ImageMetric::COSINE_COEFFICIENT:
        method = CV_TM_CCOEFF_NORMED;
        break;
    case ImageMetric::CROSS_CORRELATION:
        method = CV_TM_CCORR_NORMED;
        break;
    case ImageMetric::SQUARED_DIFFERENCES:
        method = CV_TM_SQDIFF_NORMED;
        break;
    default:
        assert(false);
        break;
    }

    // Create the result matrix
    int result_cols =  mat01.cols - mat02.cols + 1;
    int result_rows = mat01.rows - mat02.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );

    /// Do the Matching and Normalize
    matchTemplate( mat01, mat02, result, method);

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    /// For SQDIFF , the best matches are lower values. For all the other methods, the higher the better
    if(metric == ImageMetric::SQUARED_DIFFERENCES) {
        return minVal;
    }

    return maxVal;
}
