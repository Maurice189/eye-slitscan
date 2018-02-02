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

#include "cluster.h"

Cluster::Cluster(const Cluster& c1)
{
    for(const auto &e : c1.m_members) {
        m_members.push_back(e);
    }
    m_root = c1.m_root;
}

Cluster::Cluster(std::vector<int> members, node *root) : m_members(members) , m_root(root)
{

}

Cluster::Cluster(int member, const std::string &key, double distance)
{
    m_members.push_back(member);
    m_root = new node;
    m_root->key = key;
    m_root->distance = distance;
}

Cluster::~Cluster()
{
    // unique pointer would be a clearner solution here
}

Cluster Cluster::merge(const Cluster& other, double distance)
{
    std::vector<int> merged;

    merged.insert(merged.begin(), m_members.begin(), m_members.end());
    merged.insert(merged.end(), other.m_members.begin(), other.m_members.end());

    node *mergedNode = new node;

    mergedNode->left = m_root;
    mergedNode->right = other.m_root;
    mergedNode->key = std::to_string(distance);
    mergedNode->distance = distance;

    return Cluster(merged, mergedNode);
}

double leafPosX = 0;
double leafMarginX = 4;

void Cluster::print(void) const
{
    std::cout << std::endl;
    leafPosX = 0;
    postorder(m_root);
    //postorder2(m_root);
    std::cout << std::endl;
}

double Cluster::postorder2(const node *p) const
{
    if(p != NULL) {
        double leftPosX;
        double rightPosX;
        bool isLeaf = true;

        if(p->left) {
            leftPosX = postorder2(p->left);
            isLeaf = false;
        }
        if(p->right) {
            rightPosX = postorder2(p->right);
            isLeaf = false;
        }
        if (isLeaf) {
            leafPosX += leafMarginX;
            std::cout << p->key << std::setw(leafMarginX) << ' ';
            return leafPosX - leafMarginX;
        }
        assert(rightPosX > leftPosX);
        return (rightPosX + leftPosX) / 2.;
    }
}

void Cluster::postorder(const node *p, int indent) const
{
    if(p != NULL) {
        if(p->right) {
            postorder(p->right, indent+4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right) std::cout<<" /\n" << std::setw(indent) << ' ';
        std::cout<< p->key << "\n ";
        if(p->left) {
            std::cout << std::setw(indent) << ' ' <<" \\\n";
            postorder(p->left, indent+4);
        }
    }
}

node* Cluster::tree() const
{
    return m_root;
}

const std::vector<int>& Cluster::members() const
{
    return m_members;
}

unsigned int Cluster::size() const
{
    return m_members.size();
}
