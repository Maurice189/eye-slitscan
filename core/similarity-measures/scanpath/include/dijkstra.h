#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>

using namespace std;

class Graph
{
    unordered_map<int, unordered_map<int, double>> vertices;
    double distance;

public:
    void add_vertex(int name, const unordered_map<int, double>& edges)
    {
        vertices.insert(unordered_map<int, const unordered_map<int, double>>::value_type(name, edges));
    }

    void add_edge(int v1, int v2, double weight)
    {
        if (vertices.find(v1) == vertices.end()) {
            vertices[v1] = std::unordered_map<int, double>();
        }
        vertices.at(v1)[v2] = weight;
    }

    double getDistance() const
    {
        return distance;
    }

    vector<int> shortest_path(int start, int finish)
    {
        unordered_map<int, double> distances;
        unordered_map<int, int> previous;
        vector<int> nodes;
        vector<int> path;

        auto comparator = [&] (int left, int right) { return distances[left] > distances[right]; };

        for (auto& vertex : vertices)
        {
            if (vertex.first == start)
            {
                distances[vertex.first] = 0;
            }
            else
            {
                distances[vertex.first] = numeric_limits<double>::max();
            }

            nodes.push_back(vertex.first);
            push_heap(begin(nodes), end(nodes), comparator);
        }

        while (!nodes.empty())
        {
            pop_heap(begin(nodes), end(nodes), comparator);
            int smallest = nodes.back();
            nodes.pop_back();

            if (smallest == finish)
            {
                while (previous.find(smallest) != end(previous))
                {
                    path.push_back(smallest);
                    smallest = previous[smallest];
                }

                break;
            }

            if (distances[smallest] == numeric_limits<double>::max())
            {
                break;
            }

            for (auto& neighbor : vertices[smallest])
            {
                double alt = distances[smallest] + neighbor.second;
                if (alt < distances[neighbor.first])
                {
                    distances[neighbor.first] = alt;
                    previous[neighbor.first] = smallest;
                    make_heap(begin(nodes), end(nodes), comparator);
                }
            }
        }

        distance = distances[finish];
        return path;
    }
};
