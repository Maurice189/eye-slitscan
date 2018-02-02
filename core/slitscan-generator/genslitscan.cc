#include "genslitscan.h"

cv::Mat SlitscanGenerator::getVC23()
{
    return vc23;
}

cv::Mat SlitscanGenerator::getVC123()
{
    return vc123;
}

cv::Mat SlitscanGenerator::getVC1()
{
    return vc1;
}

cv::Mat SlitscanGenerator::getVC2()
{
    return vc2;
}

cv::Mat SlitscanGenerator::getVC3()
{
    return vc3;
}


cv::Scalar SlitscanGenerator::ScalarHLS2BGR(uchar H, uchar L, uchar S) {
    cv::Mat rgb;
    cv::Mat hls(1,1, CV_8UC3, cv::Scalar(H,L,S));
    cv::cvtColor(hls, rgb, CV_HLS2BGR);
    return cv::Scalar(rgb.data[0], rgb.data[1], rgb.data[2], 255);
}

cv::Mat SlitscanGenerator::createSlitScanColumn(const cv::Mat &frame, int centerX, unsigned int avgWindowRadius)
{
    if (avgWindowRadius == 0) {
        return frame.col(centerX);
    }

    cv::Mat avgColumn = cv::Mat::zeros(frame.rows, 1, CV_8UC3);

    int startPosX = centerX - avgWindowRadius;
    int endPosX = centerX + avgWindowRadius;

    if (startPosX < 0) {
        startPosX = 0;
    }

    if (endPosX >= frame.cols) {
        endPosX = frame.cols - 1;
    }

    double ncols = 1.0 + endPosX - startPosX;

    for(int posX = startPosX; posX <= endPosX; posX++) {
        avgColumn += frame.col(posX) / ncols;
    }
    return avgColumn;
}

// TODO: CHECK IF INDIX IS ALWAYS IN RANGE
cv::Mat SlitscanGenerator::createVisualization(cv::VideoCapture &inputVideo, const std::vector<struct TobiiExportData> &pData)
{
    // Frame height defines the number of rows
    int sliceRows = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
    // Frame width
    int frameWidth = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_WIDTH));
    // Frames per milliseconds
    double fpms = inputVideo.get(cv::CAP_PROP_FPS) / 1000.0;
    // Gaze index = Column index
    long gazeIndex = 0L;
    // Number of fixation points
    long nofDataPoints = pData.size();

    vc1 = cv::Mat::zeros(140, nofDataPoints, CV_8UC4);
    vc2 = cv::Mat::zeros(sliceRows, nofDataPoints, CV_8UC3);
    vc3 = cv::Mat::zeros(sliceRows, nofDataPoints, CV_8UC1);
    vc23 = cv::Mat::zeros(sliceRows, nofDataPoints, CV_8UC4);
    //vc123 = cv::Mat::zeros(sliceRows + 100, nofDataPoints, CV_8UC4);

    cv::Mat spectrogramColumn = cv::Mat::zeros(140, 1, CV_8UC4);
    cv::Mat slitScanColumn = cv::Mat::zeros(sliceRows, 1, CV_8UC3);
    cv::Mat focusMapColumn = cv::Mat::zeros(sliceRows, 1, CV_8UC1);

    cv::Mat frame;

    long timestampOffset = pData.front().timestamp;
    int currFixationIndex = -1;
    int colorIndex = 1;

    //cv::namedWindow("slitscan", cv::WINDOW_NORMAL);

    // For each eye gaze we write a single (one pixeled-wide)
    // slice to the output image
    for(long iframe = 0; inputVideo.read(frame); ++iframe) {
        struct TobiiExportData currentGaze = pData[gazeIndex];
        currentGaze.timestamp -= timestampOffset;
        int gazeAtFrame = int(std::lround(currentGaze.timestamp * fpms));

        // Go to next gaze according to the current frame
        while(gazeAtFrame < iframe) {
            //cv::imshow("slitscan", vc2);
            //cv::resizeWindow("slitscan", 700,1000);
            //cv::waitKey(0);
            if (isValid(currentGaze)) {

                if (currFixationIndex != currentGaze.fixationIndex) {
                    colorIndex *= -1;
                }

                int fromY = currentGaze.mappedGazeDataPointY - 150;
                int toY = currentGaze.mappedGazeDataPointY + 150;
                if (fromY < 0) {
                    toY += -(fromY);
                    fromY = 0;
                }
                if (toY >= sliceRows) {
                    fromY -= (toY - sliceRows);
                    toY = sliceRows-1;
                }
                focusMapColumn = cv::Mat::zeros(sliceRows, 1, CV_8UC1);
                cv::Rect rect(0, fromY, 1, 300);
                cv::Rect rect2(0, 0, 1, 100);
                cv::rectangle(focusMapColumn, rect, cv::Scalar(255));

                double xNormed = currentGaze.mappedGazeDataPointX / double(frameWidth);
                cv::Scalar color;

                if (xNormed < 0.5) {
                    color = ScalarHLS2BGR(0, 127 + (255 * xNormed), 255);
                } else {
                    color = ScalarHLS2BGR(120, 127 + (255 * (1.0 - xNormed)), 255);
                }

                cv::rectangle(spectrogramColumn, rect2, color);

                //slitScanColumn = frame.col(currentGaze.mappedGazeDataPointX);
                slitScanColumn = createSlitScanColumn(frame, currentGaze.mappedGazeDataPointX, 0);

                currFixationIndex = currentGaze.fixationIndex;
            } else {
                slitScanColumn = cv::Mat(sliceRows, 1, CV_8UC3, cv::Scalar(255,255,255));
                // Focusmap-Column stays the same ?
            }

            if (gazeIndex < nofDataPoints) {
                // Copy column from the current frame to the output image
                slitScanColumn.copyTo(vc2.col(gazeIndex));
                focusMapColumn.copyTo(vc3.col(gazeIndex));
                spectrogramColumn.copyTo(vc1.col(gazeIndex));
                gazeIndex++;
            }

            if (gazeIndex < nofDataPoints - 1) {
                currentGaze = pData[gazeIndex];
                currentGaze.timestamp -= timestampOffset;
                gazeAtFrame = int(std::lround(currentGaze.timestamp * fpms));
            } else {
                break;
            }
        }
        if (frame.empty()) {
            std::cout << "Could not open frame " << iframe << std::endl;
            throw SlitScanVideoError();
        }
    }

    std::cout << "AT GAZE INDEX " << gazeIndex << std::endl;

    cv::Mat bluredVC3a = vc3.clone();
    cv::Mat bluredVC3b = vc3.clone();

    cv::GaussianBlur(vc3, bluredVC3a, cv::Size(21, 21), 0, 0 );
    cv::GaussianBlur(vc3, bluredVC3b, cv::Size(171, 171), 0, 0 );

    vc2.copyTo(vc23, bluredVC3a);

    std::vector<cv::Mat> matChannels;
    cv::split(vc23, matChannels);

    matChannels.push_back(bluredVC3b);

    cv::merge(matChannels, vc23);

    cv::vconcat(vc1, vc23, vc123);

    //vc3 = bluredVC3b;

    return vc123;
}


VideoInfo SlitscanGenerator::readVideoInfo(const std::string &inputVideoPath)
{
    cv::VideoCapture inputVideo = cv::VideoCapture(inputVideoPath);
    if (!inputVideo.isOpened()) {
        throw SlitScanVideoError();
    }

    VideoInfo info;
    info.fpms = inputVideo.get(cv::CAP_PROP_FPS) / 1000.0;
    info.frameHeight = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
    info.frameWidth = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_WIDTH));
    info.frameCount = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_COUNT));
    info.fourCC = static_cast<int>(inputVideo.get(cv::CAP_PROP_FOURCC));

    return info;
}

cv::Mat SlitscanGenerator::createVisualization(const std::string &inputVideoPath, const std::vector<struct TobiiExportData> &pData)
{
    cv::VideoCapture capture = cv::VideoCapture(inputVideoPath);
    int sliceRows = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    if (!capture.isOpened()) {
        throw SlitScanVideoError();
    }
    return createVisualization(capture, pData);
}

bool SlitscanGenerator::isValid(const TobiiExportData &data)
{
    return data.validity < 4 &&
           data.mappedGazeDataPointX >= 0 &&
           data.mappedGazeDataPointX < data.mediaWidth &&
           data.mappedGazeDataPointY >= 0 &&
           data.mappedGazeDataPointY < data.mediaHeight;
}
