#include "participant.h"

Participant::Participant(std::vector<struct DynamicAOI> &dynamicAOIs, VideoInfo info, std::string stimuliSource)
{
    this->dynamicAOIs = dynamicAOIs;
    this->info = info;
    this->stimuliSource = stimuliSource;
}

void Participant::load(const std::string &absoluteFilePath, const std::string &label, int n, int slitScanHeight)
{
    TobiiImporter importer;

    if (!importer.read(absoluteFilePath, exportData, false)) {
        throw std::invalid_argument("Error - While parsing " + absoluteFilePath + " - Please check your file !");
    }

    SlitscanGenerator slitscanGenerator;
    slitscanGenerator.createVisualization(stimuliSource, exportData);

    cv::Mat mat = slitscanGenerator.getVC123();
    QImage image = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();

    mat = slitscanGenerator.getVC1();
    QImage imgSpectrogram = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888).rgbSwapped();

    this->color = COLORS[n % COLORS.size()];
    this->id = QString::fromStdString(label);

    this->insertIndex = n;
    this->aoiStringWTB = gazeSequenceToString(exportData, dynamicAOIs, info.fpms);

    this->matVC123 = slitscanGenerator.getVC123();
    this->matSlitscan = slitscanGenerator.getVC23();
    this->imgSlitscan = QPixmap::fromImage(image.scaled(image.width(), slitScanHeight, Qt::IgnoreAspectRatio)); // FIXME !!!!!
    this->imgSpectrogram = imgSpectrogram;
}
