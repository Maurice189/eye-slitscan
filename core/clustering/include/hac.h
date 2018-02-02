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

#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <deque>
#include <cassert>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <functional>

#include "cluster.h"

struct ClusterResult {
    int ci1;
    int ci2;

    double distance;
};

struct ClusteringResult {
    std::vector<std::pair<int, int>> steps;
    std::vector<double> distances;
    clu::node* tree;
    double sumOfDistances = 0.;
    std::vector<int> leafsOrder;
    std::vector<std::string> leafs;
};

namespace clu {
    struct CSimilarityMatrix {
        std::vector<std::vector<double>> elements;
        std::vector<std::string> labels;
    };
}


enum class ClusteringMethod
{
    SINGLE_LINKAGE,
    COMPLETE_LINKAGE,
    GROUP_AVERAGE_LINKAGE
};

class Clustering
{
private:
   class ClusterDistanceStrategy {
       // Abstract strategy method
       public :
           virtual double distance(const Cluster &c1, const Cluster &c2, const clu::CSimilarityMatrix &matrix) = 0;
   };

   class SingleLinkage : public ClusterDistanceStrategy {
       virtual double distance(const Cluster &c1, const Cluster &c2, const clu::CSimilarityMatrix &matrix)
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
   };

   class CompleteLinkage : public ClusterDistanceStrategy {
       virtual double distance(const Cluster &c1, const Cluster &c2, const clu::CSimilarityMatrix &matrix)
       {
           double max = 1e-12;

           // Complete Linkage
           for(const int m1 : c1.members()) {
               for(const int m2 : c2.members()) {
                   double dist = matrix.elements[m1][m2];
                   max = dist > max ? dist : max;
               }
           }
           return max;
       }
   };

   class GroupAverageLinkage : public ClusterDistanceStrategy {
       virtual double distance(const Cluster &c1, const Cluster &c2, const clu::CSimilarityMatrix &matrix)
       {
           double sum = 0.;

           // Group Average Linkage
           for(const int m1 : c1.members()) {
               for(const int m2 : c2.members()) {
                   double dist = matrix.elements[m1][m2];
                   sum += dist;
               }
           }
           return sum / (c1.size() * c2.size());
       }
   };
public:
    Clustering(clu::CSimilarityMatrix matrix, ClusteringMethod method=ClusteringMethod::SINGLE_LINKAGE);
    Clustering(std::vector<std::vector<double>> elements, std::vector<std::string> labels, ClusteringMethod method=ClusteringMethod::SINGLE_LINKAGE);
    struct ClusteringResult executeHAC();
    clu::node *tree();
public:
    static ClusteringResult createUniformClustering(const ClusteringResult &result);
private:
    double distance(const Cluster &c1, const Cluster &c2);
    ClusteringResult agglomerate(void);
    ClusterResult nearestNeighbors(const std::deque<Cluster> &clusters);
    bool isSymetric(const std::vector<std::vector<double>> &similarityMatrix) const;
    bool isPositive(const std::vector<std::vector<double>> &similarityMatrix) const;
    void calculateLeafsOrder(const clu::node *tree, ClusteringResult &result);

    void invertMatrix(clu::CSimilarityMatrix &matrix);
    void normalizeMatrix(clu::CSimilarityMatrix &matrix);
private:
    static void createUniformTree(node *tree, std::vector<double> &distances, const std::function<double(int)> &transform, int level=1);
private:
   clu::CSimilarityMatrix matrix;
   struct ClusteringResult cresult;
   bool invertValues;

   ClusterDistanceStrategy *clusteringStrategy;
};

#endif // CLUSTERING_H
