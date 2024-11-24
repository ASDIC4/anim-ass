// HermiteSpline.h - Header File
#ifndef HERMITE_SPLINE_H
#define HERMITE_SPLINE_H

#include <vector>
#include <array>
#include "BaseSystem.h"

class HermiteSpline : public BaseSystem {
public:
    HermiteSpline(const std::string& splineName);

    void addPoint(const double point[3], const double tangent[3]);
    void setPoint(int index, const double point[3]);
    void setTangent(int index, const double tangent[3]);
    void interpolate(float t, double result[3]);
    void computeArcLengthTable();
    void loadFromFile(const std::string& fileName);
    void exportToFile(const std::string& fileName);
    void initializeCatmullRom();
    double getArcLength(double t);
    void display(GLenum mode = GL_RENDER) override;
    int command(int argc, myCONST_SPEC char** argv) override;

private:
    std::vector<std::array<double, 3>> points;
    std::vector<std::array<double, 3>> tangents;
    std::vector<float> arcLengthTable;
    int numSamples = 100;
};

#endif // HERMITE_SPLINE_H