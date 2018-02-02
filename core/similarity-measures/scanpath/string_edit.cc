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

#include "string_edit.h"
#include <iostream>

int levenshteinDistance(const std::string& s1, const std::string& s2)
{
    // Firstly we calculate the standard Levenshtein distance
    // between the two strings.

    const size_t n(s1.length());
    const size_t m(s2.length());

    if (n == 0 || m == 0) {
        return 0.;
    }

    // We prefer heap-allocation over (dynamic) stack-allocation
    // because of possibly high dimensions (> 1000)
    unsigned int* d = new unsigned int[(n+1) * (m+1)];

    for (size_t i = 0; i <= n; ++i) {
        d[i] = i;
    }
    for (size_t j = 0; j <= m; ++j) {
        d[j*(n+1)] = j;
    }

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            unsigned int cost = s1[i-1] == s2[j-1] ? 0 : 1;
            d[i+j*(n+1)] = std::min({d[i-1+j*(n+1)]+1, d[i+(n+1)*(j-1)]+1, d[(i-1)+(n+1)*(j-1)]+cost});
        }
    }

    int distance = d[n+(n+1)*m];

    // Free used heap memory
    delete[] d;

    return distance;
}

double levenshteinScore(const std::string& s1, const std::string& s2)
{
    // Firstly we calculate the standard Levenshtein distance
    // between the two strings.

    const size_t n(s1.length());
    const size_t m(s2.length());

    if (n == 0 || m == 0) {
        return 0.;
    }

    // We prefer heap-allocation over (dynamic) stack-allocation
    // because of possibly high dimensions (> 1000)
    unsigned int* d = new unsigned int[(n+1) * (m+1)];

    for (size_t i = 0; i <= n; ++i) {
        d[i] = i;
    }
    for (size_t j = 0; j <= m; ++j) {
        d[j*(n+1)] = j;
    }

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            unsigned int cost = s1[i-1] == s2[j-1] ? 0 : 1;
            d[i+j*(n+1)] = std::min({d[i-1+j*(n+1)]+1, d[i+(n+1)*(j-1)]+1, d[(i-1)+(n+1)*(j-1)]+cost});
        }
    }

    // Based on the Levenshtein distance we can now calculate
    // the normalized similarity between the two strings.

    // Normalized dissimilarity
    double normalized = static_cast<double>(d[n+(n+1)*m]) / std::max(n, m);

    // Free used heap memory
    delete[] d;

    // Return normalized similarity
    return 1. - normalized;
}


double needlemanWunschScore(const std::string& s1,
                            const std::string& s2,
                            const std::function<double(char, char)>& subWeights,
                            int gapValue)
{
    const size_t n(s1.length());
    const size_t m(s2.length());

    if (n == 0 || m == 0) {
        return gapValue;
    }

    int* f = new int[(n+1)*(m+1)];

    for (size_t i = 0; i <= n; ++i) {
        f[i] = i * gapValue;
    }
    for (size_t j = 0; j <= m; ++j) {
        f[j*(n+1)] = j * gapValue;
    }

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            int cost = subWeights(s1[i-1], s2[j-1]);
            f[i+j*(n+1)] = std::max({f[i-1+j*(n+1)]+gapValue, f[i+(j-1)*(n+1)]+gapValue, f[i-1+(j-1)*(n+1)]+cost});
        }
    }

    // Unnormalized score
    double score = f[n+m*(n+1)];

    // Free used heap memory
    delete[] f;

    return score;
}

double needlemanWunschScore(const std::string& s1,
                            const std::string& s2,
                            const std::function<double(char, char)>& subWeights,
                            int maxSubWeight,
                            int gapValue,
                            double normalized)
{
    const size_t n(s1.length());
    const size_t m(s2.length());

    if(maxSubWeight == 0) {
        // TODO: Handle this case !
       return 0.0;
    }

    if (n == 0 || m == 0) {
        return gapValue / (normalized ? double(maxSubWeight) : 1);
    }

    int* f = new int[(n+1)*(m+1)];

    for (size_t i = 0; i <= n; ++i) {
        f[i] = i * gapValue;
    }
    for (size_t j = 0; j <= m; ++j) {
        f[j*(n+1)] = j * gapValue;
    }

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            int cost = subWeights(s1[i-1], s2[j-1]);
            f[i+j*(n+1)] = std::max({f[i-1+j*(n+1)]+gapValue, f[i+(j-1)*(n+1)]+gapValue, f[i-1+(j-1)*(n+1)]+cost});
        }
    }

    // Unnormalized score
    double score = f[n+m*(n+1)];

    if (normalized) {
        // Based on the Needleman Wunsch score we can now calculate
        // the normalized similarity between the two strings.
        // We might get negative values,
        // but thats the nature of NW due to the choosen weights/gapValue.
        score /= (maxSubWeight * std::max(n, m));
    }

    // Free used heap memory
    delete[] f;

    return score;
}
