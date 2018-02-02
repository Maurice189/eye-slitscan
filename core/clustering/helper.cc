#include "helper.h"

void invertMatrix(std::vector<std::vector<double>> &elements)
{
    for(int i = 0; i < elements.size(); i++) {
        for (int j = i+1; j < elements[i].size(); j++) {
            elements[i][j] = 1. / (elements[i][j]);
            elements[j][i] = 1. / (elements[j][i]);
        }
    }
}

void normalizeMatrix(std::vector<std::vector<double>> &elements)
{
    double max = 0.;
    for(int i = 0; i < elements.size(); i++) {
        for (int j = i+1; j < elements[i].size(); j++) {
            if (elements[i][j] > max) {
                max = elements[i][j];
            }
        }
    }
    for(int i = 0; i < elements.size(); i++) {
        for (int j = i+1; j < elements[i].size(); j++) {
            elements[i][j] = elements[i][j] / max;
            elements[j][i] = elements[j][i] / max;
        }
    }
}

void createPositiveMatrix(std::vector<std::vector<double>> &elements)
{
    double min = 1e12;
    for(int i = 0; i < elements.size(); i++) {
        for (int j = i+1; j < elements[i].size(); j++) {
            if (elements[i][j] < min) {
                min = elements[i][j];
            }
        }
    }
    // Shift towards positive values, if neccesarry
    if (min < 0) {
        std::cerr << "Shift values of towards zero !" << std::endl;
        // Shift makes it positive and far away from zero.
        double shift = -2. * min;
        for(int i = 0; i < elements.size(); i++) {
            for (int j = i+1; j < elements[i].size(); j++) {
                double score = elements[i][j] + shift;
                elements[i][j] = score;
                elements[j][i] = score;
            }
        }
    }
}
