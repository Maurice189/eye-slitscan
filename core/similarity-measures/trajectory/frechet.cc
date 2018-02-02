#include "frechet.h"

double discreteFrechetDistance(const struct Trajectory& trP1,
                               const struct Trajectory& trP2)
{
    int n = trP1.points.size();
    int m = trP2.points.size();

    std::vector<std::vector<double>> table(n, std::vector<double>(m));

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            table[i][j] = -1.;
        }
    }
    /*
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            std::cout << table[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
    double result = rc(trP1, trP2, table, n-1, m-1);
    /*
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            std::cout << table[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
    return result;
}

double rc(const struct Trajectory& trP1,
          const struct Trajectory& trP2,
          std::vector<std::vector<double>> &table,
          int i, int j)
{
    if (table[i][j] > -1) {
        return table[i][j];
    }
    else if(i == 0 && j == 0) {
        table[i][j] = euclDist(trP1.points[i], trP2.points[j]);
    }
    else if(i > 0 && j == 0) {
        table[i][j] = std::max(rc(trP1, trP2, table, i-1, 0),
                               euclDist(trP1.points[i], trP2.points[j]));

    }
    else if (i == 0 && j > 0) {
        table[i][j] = std::max(rc(trP1, trP2, table, 0, j-1),
                               euclDist(trP1.points[i], trP2.points[j]));

    }
    else if (i > 0 && j > 0) {
        table[i][j] = std::max(std::min({
                                   rc(trP1, trP2, table, i-1, j),
                                   rc(trP1, trP2, table, i-1, j-1),
                                   rc(trP1, trP2, table, i, j-1)}),
                               euclDist(trP1.points[i], trP2.points[j]));
    }
    else {
        table[i][j] = std::numeric_limits<double>::max();
    }
    return table[i][j];
}

double euclDist(const struct std::pair<double, double> &p1, const std::pair<double, double> &p2)
{
    double distX = p1.first - p2.first;
    double distY = p1.second - p2.second;

    return std::fabs(distX) + std::fabs(distY);
    //return std::sqrt(std::pow(distX, 2) + std::pow(distY, 2));
}
