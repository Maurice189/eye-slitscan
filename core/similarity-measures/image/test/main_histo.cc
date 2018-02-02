#include <iostream>
#include <QImage>
#include "histo-measure.h"


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

    std::cout << "-------------------- Correlation --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << histogramScore(img01, img02, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << histogramScore(img01, img03, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << histogramScore(img02, img03, HistogramMetric::CORRELATION) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << histogramScore(img04, img05, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << histogramScore(img04, img06, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << histogramScore(img05, img06, HistogramMetric::CORRELATION) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << histogramScore(img07, img08, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << histogramScore(img07, img09, HistogramMetric::CORRELATION) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << histogramScore(img08, img09, HistogramMetric::CORRELATION) << std::endl;

    std::cout << "-------------------- Chi-Square --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << histogramScore(img01, img02, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << histogramScore(img01, img03, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << histogramScore(img02, img03, HistogramMetric::CHI_SQUARE) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << histogramScore(img04, img05, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << histogramScore(img04, img06, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << histogramScore(img05, img06, HistogramMetric::CHI_SQUARE) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << histogramScore(img07, img08, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << histogramScore(img07, img09, HistogramMetric::CHI_SQUARE) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << histogramScore(img08, img09, HistogramMetric::CHI_SQUARE) << std::endl;

    std::cout << "-------------------- Intersection --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << histogramScore(img01, img02, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << histogramScore(img01, img03, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << histogramScore(img02, img03, HistogramMetric::INTERSECTION) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << histogramScore(img04, img05, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << histogramScore(img04, img06, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << histogramScore(img05, img06, HistogramMetric::INTERSECTION) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << histogramScore(img07, img08, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << histogramScore(img07, img09, HistogramMetric::INTERSECTION) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << histogramScore(img08, img09, HistogramMetric::INTERSECTION) << std::endl;

    std::cout << "-------------------- Bhattacharyya distance --------------------" << std::endl;
    std::cout << "scan01_p1 ~ scan01_p2 = " << histogramScore(img01, img02, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan01_p1 ~ scan01_p3 = " << histogramScore(img01, img03, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan01_p2 ~ scan01_p3 = " << histogramScore(img02, img03, HistogramMetric::BHATTACHARYYA) << std::endl;

    std::cout << "scan02_p1 ~ scan02_p2 = " << histogramScore(img04, img05, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan02_p1 ~ scan02_p3 = " << histogramScore(img04, img06, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan02_p2 ~ scan02_p3 = " << histogramScore(img05, img06, HistogramMetric::BHATTACHARYYA) << std::endl;

    std::cout << "scan03_p1 ~ scan03_p2 = " << histogramScore(img07, img08, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan03_p1 ~ scan03_p3 = " << histogramScore(img07, img09, HistogramMetric::BHATTACHARYYA) << std::endl;
    std::cout << "scan03_p2 ~ scan03_p3 = " << histogramScore(img08, img09, HistogramMetric::BHATTACHARYYA) << std::endl;

    return 0;
}
