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


#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include <iomanip>

class Cluster;


namespace clu {
    struct node {
        node *left = nullptr;
        node *right = nullptr;
        std::string key = "";
        double distance = 0.;

        ~node()
        {
            if (left) {
                delete left;
            }
            if(right) {
                delete right;
            }
        }

        int depth(int level=0)
        {
            int leftDepth = level;
            int rightDepth = level;

            if (left) {
                leftDepth = left->depth(level+1);
            }
            if(right) {
                rightDepth = right->depth(level+1);
            }

            return std::max(leftDepth, rightDepth);
        }

        static void deepCopy(const node *src, node* &dest)
        {
            if (src) {
                dest = new clu::node;

                dest->key = src->key;
                dest->distance = src->distance;

                deepCopy(src->left, dest->left);
                deepCopy(src->right, dest->right);
            }
        }
    };
}

using namespace clu;

class Cluster
{
public:
    Cluster(const Cluster& c1);
    Cluster(std::vector<int> members, node *root);
    Cluster(int member, const std::string &key, double distance=0.);
    ~Cluster();

    Cluster merge(const Cluster& other, double distance);
    const std::vector<int>& members() const;
    unsigned int size() const;
    node* tree() const;
    void print(void) const;
private:
    void postorder(const node *p, int indent=0) const;
    double postorder2(const node *p) const;
private:
    std::vector<int> m_members;
    node *m_root;

};

#endif // CLUSTER_H
