#include "image-measure.h"
#include <QImage>
#include "QtOpenCV/cvmatandqimage.h"

int main(void)
{
    QImage img01("res/scan01_p1.png");
    QImage img02("res/scan01_p2.png");
    QImage img03("res/scan01_p3.png");

    QImage img04("res/scan02_p1.png");
    QImage img05("res/scan02_p2.png");
    QImage img06("res/scan02_p3.png");

    QImage img07("res/scan03_p1.png");
    QImage img08("res/scan03_p2.png");
    QImage img09("res/scan03_p3.png");

    cv::Mat imgMat01 = QtOcv::image2Mat_shared(img01);
    cv::Mat imgMat02 = QtOcv::image2Mat_shared(img02);
    cv::Mat imgMat03 = QtOcv::image2Mat_shared(img03);

    cv::Mat imgMat04 = QtOcv::image2Mat_shared(img04);
    cv::Mat imgMat05 = QtOcv::image2Mat_shared(img05);
    cv::Mat imgMat06 = QtOcv::image2Mat_shared(img06);

    cv::Mat imgMat07 = QtOcv::image2Mat_shared(img07);
    cv::Mat imgMat08 = QtOcv::image2Mat_shared(img08);
    cv::Mat imgMat09 = QtOcv::image2Mat_shared(img09);

    std::cout << "-------------------- CV_TM_SQDIFF --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_SQDIFF) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_SQDIFF) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_SQDIFF) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_SQDIFF) << std::endl;

    std::cout << "-------------------- CV_TM_SQDIFF_NORMED --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_SQDIFF_NORMED) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_SQDIFF_NORMED) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_SQDIFF_NORMED) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_SQDIFF_NORMED) << std::endl;

    std::cout << "-------------------- CV_TM_CCORR --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_CCORR) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_CCORR) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_CCORR) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_CCORR) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_CCORR) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_CCORR) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_CCORR) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_CCORR) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_CCORR) << std::endl;

    std::cout << "-------------------- CV_TM_CCORR_NORMED --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_CCORR_NORMED) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_CCORR_NORMED) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_CCORR_NORMED) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_CCORR_NORMED) << std::endl;

    std::cout << "-------------------- CV_TM_CCOEFF --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_CCOEFF) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_CCOEFF) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_CCOEFF) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_CCOEFF) << std::endl;

    std::cout << "-------------------- CV_TM_CCOEFF_NORMED --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << compareImage(imgMat01, imgMat02, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << compareImage(imgMat01, imgMat03, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << compareImage(imgMat02, imgMat03, CV_TM_CCOEFF_NORMED) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << compareImage(imgMat04, imgMat05, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << compareImage(imgMat04, imgMat06, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << compareImage(imgMat05, imgMat06, CV_TM_CCOEFF_NORMED) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << compareImage(imgMat07, imgMat08, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << compareImage(imgMat07, imgMat09, CV_TM_CCOEFF_NORMED) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << compareImage(imgMat08, imgMat09, CV_TM_CCOEFF_NORMED) << std::endl;

    return 0;
}

