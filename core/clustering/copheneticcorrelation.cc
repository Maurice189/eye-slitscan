#include "copheneticcorrelation.h"

#include <cmath>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>

#include <iostream>

CopheneticCorrelation::CopheneticCorrelation(const ClusteringResult &r1, const ClusteringResult &r2, const std::vector<std::string> labels, int nofClasses)
{
    double classDistance = nofClasses;

    copheneticValue = [classDistance](double x){
        return std::floor(classDistance * x);
    };

    std::function<double(double)> distTransformR1;
    std::function<double(double)> distTransformR2;

    double maxR1 = *std::max_element(r1.distances.begin(), r1.distances.end());
    double maxR2 = *std::max_element(r2.distances.begin(), r2.distances.end());

    distTransformR1 = [maxR1](double x) {return x / maxR1;};
    distTransformR2 = [maxR2](double x) {return x / maxR2;};

    std::vector<std::vector<int>> cphMatrixR1, cphMatrixR2;

    for(int i = 0; i < labels.size(); i++) {
        toIndex[labels[i]] = i;

        std::vector<int> c1(labels.size());
        std::vector<int> c2(labels.size());

        cphMatrixR1.push_back(c1);
        cphMatrixR2.push_back(c2);
    }

    calculateCopheneticMatrix(r1.tree, cphMatrixR1, distTransformR1);
    calculateCopheneticMatrix(r2.tree, cphMatrixR2, distTransformR2);

    listR1 = flatten(cphMatrixR1);
    listR2 = flatten(cphMatrixR2);
}


double CopheneticCorrelation::correlationCoefficient(bool normalized) const
{
    double corr = ppmcc(listR1, listR2);
    // Pearson is between -1 and 1 per definition
    // so we need T(a) = (a+1) / 2 for normalization.
    return normalized ? (corr + 1.) / 2. : corr;
}

void CopheneticCorrelation::leafsOfTree(node *tree, std::vector<int> &leafs)
{
    if (tree) {
        bool isLeaf = true;
        if (tree->left) {
            leafsOfTree(tree->left, leafs);
            isLeaf = false;
        }
        if (tree->right) {
            leafsOfTree(tree->right, leafs);
            isLeaf = false;
        }
        if (isLeaf) {
            leafs.push_back(toIndex[tree->key]);
        }
    }
}

void CopheneticCorrelation::calculateCopheneticMatrix(clu::node *tree, std::vector<std::vector<int>> &matrix, const std::function<double(double)> &distTransform)
{
    if (tree) {
        bool isLeaf = true;
        if (tree->left) {
            calculateCopheneticMatrix(tree->left, matrix, distTransform);
            isLeaf = false;
        }
        if (tree->right) {
            calculateCopheneticMatrix(tree->right, matrix, distTransform);
            isLeaf = false;
        }
        if (!isLeaf) {
            std::vector<int> leftLeafs, rightLeafs;

            leafsOfTree(tree->left, leftLeafs);
            leafsOfTree(tree->right, rightLeafs);

            //double distance = distTransform(std::stof((tree->key)));
            double distance = distTransform(tree->distance);
            int value = copheneticValue(distance);

            for(auto left : leftLeafs) {
                for(auto right : rightLeafs) {
                    matrix[left][right] = value;
                    matrix[right][left] = value;
                }
            }
        }
    }
}


std::vector<int> CopheneticCorrelation::flatten(const std::vector<std::vector<int> > &matrix)
{
    std::vector<int> vec;
    for(int i = 0; i < matrix.size(); i++) {
        for (int j = i+1; j < matrix[i].size(); j++) {
            vec.push_back(matrix[i][j]);
        }
    }
    return vec;
}


double CopheneticCorrelation::sampleMean(const std::vector<int> &ds) const
{
    int n = ds.size();
    int sum = 0;

    for(auto e : ds) {
        sum += e;
    }

    return double(sum) / n;
}


double CopheneticCorrelation::squaredDifferences(const std::vector<int> &ds, double mean) const
{
    double var = 0.;
    for (auto e : ds) {
        var += std::pow(e - mean, 2);
    }

    return std::sqrt(var);
}

double CopheneticCorrelation::ppmcc(const std::vector<int> &ds1, const std::vector<int> &ds2) const
{
    assert(ds1.size() == ds2.size());
    assert(!ds1.empty());

    int n = ds1.size();

    double ds1Mean = sampleMean(ds1);
    double ds2Mean = sampleMean(ds2);

    double ds1Std = squaredDifferences(ds1, ds1Mean);
    double ds2Std = squaredDifferences(ds2, ds2Mean);

    double cov = 0.;

    for(int i = 0; i < n; i++) {
        cov += ((ds1[i] - ds1Mean) * (ds2[i] - ds2Mean));
    }

    return cov / (ds1Std * ds2Std);
}
