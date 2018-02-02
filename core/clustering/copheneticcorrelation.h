#ifndef COPHENETICCORRELATION_H
#define COPHENETICCORRELATION_H

#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <map>
#include <string>
#include <iostream>

#include "include/hac.h"

class CopheneticCorrelation
{
public:
    CopheneticCorrelation(const ClusteringResult &r1, const ClusteringResult &r2, const std::vector<std::string> labels, int nofClasses=50);
    double correlationCoefficient(bool normalized=false) const;
    std::vector<int> copheneticValuesR1()
    {
        return listR1;
    }

    std::vector<int> copheneticValuesR2()
    {
        return listR2;
    }

private:
    void leafsOfTree(clu::node *tree, std::vector<int> &leafs);
    void calculateCopheneticMatrix(clu::node *tree, std::vector<std::vector<int> > &matrix, const std::function<double(double)> &distTransform);

    std::vector<int> flatten(const std::vector<std::vector<int>> &matrix);

    double ppmcc(const std::vector<int> &ds1, const std::vector<int> &ds2) const;
    double sampleMean(const std::vector<int> &ds) const;
    double squaredDifferences(const std::vector<int> &ds, double mean) const;
private:
    std::function<int(double)> copheneticValue;
    std::map<std::string, int> toIndex;
    std::vector<int> listR1, listR2;

};

#endif // COPHENETICCORRELATION_H
