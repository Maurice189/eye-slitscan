#ifndef CSVEXPORT_H
#define CSVEXPORT_H

#include "constants.h"
#include <QList>
#include <string>

class CSVExport
{
public:
    void writeCophenticCorrelationToFile(bool uniform, std::string filename);
    void writeDistanceMatricesToCSV(std::string filename);
    CSVExport(const QList<ClusteringStateData> &data, const GenericSourceInfo &srcInfo);
private:
    std::string createDetailedMeasureInfo();
private:
    const QList<ClusteringStateData> &data;
    const GenericSourceInfo &srcInfo;
};

#endif // CSVEXPORT_H
