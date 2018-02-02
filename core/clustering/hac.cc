// Copyright: (2017) Maurice Koch
// License: BSD-3
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "hac.h"

#include <algorithm>
#include <climits>

// Similarity matrix has to be symetric !
Clustering::Clustering(CSimilarityMatrix cmatrix, ClusteringMethod method)
{
    this->matrix = cmatrix;

    if (matrix.elements.empty()) {
        throw std::invalid_argument("clustering ::: similarity-matrix must not be empty !");
    }

    // Front element exists due to previous check
    if (matrix.elements.size() != matrix.elements.front().size()) {
        throw std::invalid_argument("clustering ::: similarity-matrix must not have different dimensions !");
    }

    if (!isSymetric(matrix.elements)) {
        throw std::invalid_argument("clustering ::: similarity-matrix has to be symetric !");
    }

    if (method == ClusteringMethod::SINGLE_LINKAGE) {
        clusteringStrategy = new SingleLinkage;
    } else if (method == ClusteringMethod::GROUP_AVERAGE_LINKAGE) {
        clusteringStrategy = new GroupAverageLinkage;
    } else if (method == ClusteringMethod::COMPLETE_LINKAGE) {
        clusteringStrategy = new CompleteLinkage;
    }
}

Clustering::Clustering(std::vector<std::vector<double>> elements, std::vector<std::string> labels, ClusteringMethod method)
{
    matrix.elements = elements;
    matrix.labels = labels;

    if (matrix.elements.empty()) {
        throw std::invalid_argument("clustering ::: similarity-matrix must not be empty !");
    }

    // Front element exists due to previous check
    if (matrix.elements.size() != matrix.elements.front().size()) {
        throw std::invalid_argument("clustering ::: similarity-matrix must not have different dimensions !");
    }

    if (!isSymetric(matrix.elements)) {
        throw std::invalid_argument("clustering ::: similarity-matrix has to be symetric !");
    }

    if (method == ClusteringMethod::SINGLE_LINKAGE) {
        clusteringStrategy = new SingleLinkage;
    } else if (method == ClusteringMethod::GROUP_AVERAGE_LINKAGE) {
        clusteringStrategy = new GroupAverageLinkage;
    } else if (method == ClusteringMethod::COMPLETE_LINKAGE) {
        clusteringStrategy = new CompleteLinkage;
    }
}

void Clustering::invertMatrix(CSimilarityMatrix &matrix)
{
    for(int i = 0; i < matrix.elements.size(); i++) {
        for (int j = 0; j < matrix.elements[i].size(); j++) {
            matrix.elements[i][j] = matrix.elements[i][j] > 0. ? 1. / (matrix.elements[i][j]) : std::numeric_limits<double>::max();
        }
    }
}

void Clustering::normalizeMatrix(CSimilarityMatrix &matrix)
{
    double max = 0.;
    for(auto row : matrix.elements) {
        for (auto e : row) {
            if (e > max) {
                max = e;
            }
        }
    }
    for(int i = 0; i < matrix.elements.size(); ++i) {
        for(int j = 0; j < matrix.elements.size(); ++j) {
            matrix.elements[i][j] = max > 0. ? matrix.elements[i][j] / max : 0.;
        }
    }
}

bool Clustering::isSymetric(const std::vector<std::vector<double>> &similarityMatrix) const
{
    for(int i = 0; i < similarityMatrix.size(); ++i) {
        for(int j = 0; j < similarityMatrix.size(); ++j) {
            if (similarityMatrix[i][j] != similarityMatrix[j][i]) {
                return false;
            }
        }
    }
    return true;
}

bool Clustering::isPositive(const std::vector<std::vector<double>> &similarityMatrix) const
{
    for(int i = 0; i < similarityMatrix.size(); ++i) {
        for(int j = 0; j < similarityMatrix.size(); ++j) {
            if (similarityMatrix[i][j] < 0.) {
                return false;
            }
        }
    }
    return true;
}


struct ClusteringResult Clustering::executeHAC()
{
    return agglomerate();
}

/* Complexity : ?? * (log n) */
ClusteringResult Clustering::agglomerate()
{
    assert(clusteringStrategy != nullptr);

    ClusteringResult cresult;
    std::deque<Cluster> clusters;
    for(int member = 0; member < matrix.elements.size(); ++member) {
        clusters.push_back(Cluster(member, matrix.labels[member], 0.));
    }

    /* We have log n merges */
    while (clusters.size() > 1) {
        auto result = nearestNeighbors(clusters);

        assert(result.ci1 != -1 && result.ci2 != -1);
        assert(result.ci1 != result.ci2);

        double distance = result.distance;

        Cluster merged = clusters[result.ci1].merge(clusters[result.ci2], distance);

        cresult.sumOfDistances += result.distance;
        cresult.steps.push_back(std::pair<int, int>(result.ci1, result.ci2));
        cresult.distances.push_back(result.distance);

        // First remove the element that has a greater index, otherwise
        // erasement could lead to wrong results, due to changed indices.
        if (result.ci1 < result.ci2) {
            clusters.erase(clusters.begin() + result.ci2);
            clusters.erase(clusters.begin() + result.ci1);
        } else {
            clusters.erase(clusters.begin() + result.ci1);
            clusters.erase(clusters.begin() + result.ci2);
        }

        clusters.push_back(merged);
    }

    assert(clusters.size() == 1);

    clusters.front().print();
    // FIXME : That's bad
    //delete clusters.front().tree();
    cresult.tree = clusters.front().tree();
    calculateLeafsOrder(cresult.tree, cresult);
    return cresult;
}

/* Complexitiy : O(n^4) ? Is it even in O(n^3) ?? */
ClusterResult Clustering::nearestNeighbors(const std::deque<Cluster> &clusters)
{
    if(clusters.size() < 2) {
        throw std::invalid_argument("clustering ::: at least two clusters are "
                                    "needed for nearest neighbors");
    }

    //double minDistance = 1. / (distance(clusters[0], clusters[1])+1e-9);
    double minDistance = clusteringStrategy->distance(clusters[0], clusters[1], matrix);
    std::pair<int, int> indices {0, 1};

    for(int i = 0; i < clusters.size(); ++i) {
        for (int j = i+1; j < clusters.size(); j++) {
            //double dist = distance(clusters[i], clusters[j]);
            double dist = clusteringStrategy->distance(clusters[i], clusters[j], matrix);
            if (dist < minDistance) {
                indices.first = i;
                indices.second = j;
                minDistance = dist;
            }
        }
    }
    cresult.steps.push_back(indices);
    cresult.distances.push_back(minDistance);

    return ClusterResult{indices.first, indices.second, minDistance};
}

double Clustering::distance(const Cluster &c1, const Cluster &c2)
{
    double min = 1e12;

    // Single Linkage
    for(const int m1 : c1.members()) {
        for(const int m2 : c2.members()) {
            double dist = matrix.elements[m1][m2];
            min = dist < min ? dist : min;
        }
    }
    return min;
}

void Clustering::calculateLeafsOrder(const clu::node *tree, ClusteringResult &result)
{
   if (tree != nullptr) {
        bool isNotLeaf = tree->left || tree->right;

        if (isNotLeaf) {
            calculateLeafsOrder(tree->left, result);
            calculateLeafsOrder(tree->right, result);
        } else {
            for(int i = 0; i < matrix.labels.size(); ++i) {
                if (tree->key == matrix.labels[i]) {
                    result.leafsOrder.push_back(i);
                    break;
                }
            }
            result.leafs.push_back(tree->key);
        }
   }
}

ClusteringResult Clustering::createUniformClustering(const ClusteringResult &result)
{
    ClusteringResult uniformResult;

    // This will be the same - We do change the values, but not the order !
    uniformResult.leafs = result.leafs;
    uniformResult.leafsOrder = result.leafsOrder;
    uniformResult.steps = result.steps;

    clu::node::deepCopy(result.tree, uniformResult.tree);

    double min = *std::min_element(result.distances.begin(), result.distances.end());
    double max = *std::max_element(result.distances.begin(), result.distances.end());

    int depth = result.tree->depth();

    double a = (min - max) / (depth - 1);
    double b = max - a;

    auto uniform = [a, b](int level) {
        return (a * level) + b;
    };

    createUniformTree(uniformResult.tree, uniformResult.distances, uniform);

    uniformResult.sumOfDistances = 0.;
    for(double distance : uniformResult.distances) {
        uniformResult.sumOfDistances += distance;
    }

    return uniformResult;
}

void Clustering::createUniformTree(clu::node *tree, std::vector<double> &distances, const std::function<double(int)> &transform, int level)
{
    if(tree) {
        bool isLeaf = true;
        if(tree->left) {
            isLeaf = false;
            createUniformTree(tree->left, distances, transform, level+1);
        }
        if(tree->right) {
            isLeaf = false;
            createUniformTree(tree->right, distances, transform, level+1);
        }
        if (!isLeaf) {
            double distance = transform(level);

            distances.push_back(distance);
            tree->distance = distance;
        }
    }

}
