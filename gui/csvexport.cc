#include "csvexport.h"
#include "../core/clustering/copheneticcorrelation.h"
#include <fstream>

CSVExport::CSVExport(const QList<ClusteringStateData> &data, const GenericSourceInfo &srcInfo)
    : data(data), srcInfo(srcInfo)
{

}

void CSVExport::writeDistanceMatricesToCSV(std::string filename)
{
    std::string viewerString = "";

    for(const auto &viewer : data[0].matrix.labels) {
        viewerString += viewer.toStdString();
        viewerString += " , ";
    }
    std::string header;

    header =  "General Info    :\n";
    header += "Video Source    : " + srcInfo.filename + "\n";
    header += "Viewers         : " + viewerString + "\n";
    header += "Measure Details : " + createDetailedMeasureInfo() + "\n\n";

    int n = data[0].matrix.elements.size();
    auto labels = data[0].matrix.labels;

    std::ofstream csvFile;
    csvFile.open (filename, std::fstream::trunc | std::fstream::out);

    csvFile << header;
    csvFile << "PAB,";

    for(int i = 0; i < data.size(); i++) {
        csvFile << data[i].matrix.name.toUpper().replace(" ", "_").replace("-", "_").toStdString();
        if (i < data.size()-1) {
            csvFile << ',';
        }
    }
    csvFile << std::endl;
    for(int x = 0; x < n; x++) {
        for(int y = x+1; y < n; y++) {
            csvFile << labels[x].toStdString() << "_" << labels[y].toStdString() << ",";
            for(int i = 0; i < data.size(); i++) {
                csvFile << QString::number(data[i].matrix.elements[x][y]).replace(",", ".").toStdString();
                if (i < data.size()-1) {
                    csvFile << ',';
                }
            }
            csvFile << std::endl;
        }
    }

    csvFile.close();
}

std::string CSVExport::createDetailedMeasureInfo()
{
    std::string infoStr = "";

    for(const auto &state : data) {
        infoStr += state.matrix.name.toUpper().replace(" ", "_").replace("-", "_").toStdString();
        infoStr += "[";
        for (const auto &value : state.matrix.info.properties.values()) {
            infoStr += value.toStdString();
            infoStr += ",";
        }
        infoStr += "]  ";
    }
    return infoStr;
}

void CSVExport::writeCophenticCorrelationToFile(bool uniform, std::string filename)
{
    std::ofstream csvFile;
    csvFile.open (filename, std::fstream::trunc | std::fstream::out);

    csvFile << std::endl << ",";
    for(int i = 0; i < data.size(); i++) {
        csvFile << data[i].info.info.title.toStdString() << ",";
    }
    csvFile << std::endl;

    for(int i = 0; i < data.size(); i++) {
        csvFile << data[i].info.info.title.toStdString() << ",";
        for(int j = 0; j < data.size(); j++) {
            if (uniform) {
                CopheneticCorrelation corr(Clustering::createUniformClustering(data[i].result), Clustering::createUniformClustering(data[j].result), data[i].result.leafs, 100);
                csvFile << corr.correlationCoefficient(true) << ",";

            } else {
                CopheneticCorrelation corr(data[i].result, data[j].result, data[i].result.leafs, 100);
                csvFile << corr.correlationCoefficient(true) << ",";
            }
        }
        csvFile << std::endl;
    }
    csvFile.close();
}
