
// HermiteSpline.cpp - Class Implementation
#include "HermiteSpline.h"
#include "SampleParticle.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include "GlobalResourceManager.h"
#include "global.h"

HermiteSpline::HermiteSpline(const std::string& splineName) : BaseSystem(splineName) {}

void HermiteSpline::addPoint(const double point[3], const double tangent[3]) {
    if (points.size() < 100) {
        std::array<double, 3> newPoint = { point[0], point[1], point[2] };
        std::array<double, 3> newTangent = { tangent[0], tangent[1], tangent[2] };
        points.push_back(newPoint);
        tangents.push_back(newTangent);
        computeArcLengthTable();
    }
    else {
        std::cout << "Max control points reached!" << std::endl;
    }
}

void HermiteSpline::setTangent(int index, const double tangent[3]) {
    if (index >= 0 && index < tangents.size()) {
        tangents[index] = { tangent[0], tangent[1], tangent[2] };
        computeArcLengthTable();
    }
}

void HermiteSpline::setPoint(int index, const double point[3]) {
    if (index >= 0 && index < points.size()) {
        points[index] = { point[0], point[1], point[2] };
        computeArcLengthTable();
    }
}


void HermiteSpline::computeArcLengthTable() {
    arcLengthTable.clear();  // 清空弧长表
    arcLengthTable.push_back(0.0f);  // 第一个点弧长为0

    double totalLength = 0.0;
    int numSamples = 100;  // 将 [0, 1] 区间分成100个小段

    // 遍历每一段样条曲线
    for (int i = 0; i < points.size() - 1; ++i) {
        for (int j = 0; j < numSamples; ++j) {
            float t1 = static_cast<float>(j) / numSamples;
            float t2 = static_cast<float>(j + 1) / numSamples;

            // 计算 t1 和 t2 对应的曲线点
            double point1[3], point2[3];
            interpolate(static_cast<float>(i) + t1, point1);  // Hermite 样条插值计算曲线点
            interpolate(static_cast<float>(i) + t2, point2);

            // 计算两点间的欧几里得距离作为弧长的一部分
            double segmentLength = sqrt(pow(point2[0] - point1[0], 2) +
                pow(point2[1] - point1[1], 2) +
                pow(point2[2] - point1[2], 2));

            totalLength += segmentLength;
            arcLengthTable.push_back(totalLength);  // 存储当前弧长
        }
    }
}

void HermiteSpline::interpolate(float t, double result[3]) {
    int i0 = static_cast<int>(t);
    int i1 = i0 + 1;
    if (i1 >= points.size()) {
        result[0] = points.back()[0];
        result[1] = points.back()[1];
        result[2] = points.back()[2];
        return;
    }

    float localT = t - static_cast<float>(i0);
    std::array<double, 3> p0 = points[i0];
    std::array<double, 3> p1 = points[i1];
    std::array<double, 3> t0 = tangents[i0];
    std::array<double, 3> t1 = tangents[i1];

    float h00 = 2.0f * localT * localT * localT - 3.0f * localT * localT + 1.0f;
    float h10 = localT * localT * localT - 2.0f * localT * localT + localT;
    float h01 = -2.0f * localT * localT * localT + 3.0f * localT * localT;
    float h11 = localT * localT * localT - localT * localT;

    result[0] = h00 * p0[0] + h10 * t0[0] + h01 * p1[0] + h11 * t1[0];
    result[1] = h00 * p0[1] + h10 * t0[1] + h01 * p1[1] + h11 * t1[1];
    result[2] = h00 * p0[2] + h10 * t0[2] + h01 * p1[2] + h11 * t1[2];
}

void HermiteSpline::loadFromFile(const std::string& fileName) {
    std::ifstream inFile(fileName);
    if (!inFile) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    points.clear();
    tangents.clear();

    std::string splineName;
    int numPoints;
    inFile >> splineName >> numPoints;
    
    for (int i = 0; i < numPoints; ++i) {
        std::array<double, 3> point, tangent;
        inFile >> point[0] >> point[1] >> point[2] >> tangent[0] >> tangent[1] >> tangent[2];
        addPoint(point.data(), tangent.data());

        animTcl::OutputMessage("Added point to hermite spline '%s' (%d/%d)", splineName.c_str(), i + 1, numPoints);
        animTcl::OutputMessage("        location: [%f, %f, %f]", point[0], point[1], point[2]);
        animTcl::OutputMessage("        tangent: [%f, %f, %f]", tangent[0], tangent[1], tangent[2]);
    }
    animTcl::OutputMessage("Imported spline '%s' with %d points from file '%s'.", splineName.c_str(), numPoints, fileName.c_str());

    inFile.close();

    
    computeArcLengthTable();
}

void HermiteSpline::exportToFile(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    outFile << retrieveName() << " " << points.size() << "\n";
    for (size_t i = 0; i < points.size(); ++i) {
        outFile << points[i][0] << " " << points[i][1] << " " << points[i][2] << " "
            << tangents[i][0] << " " << tangents[i][1] << " " << tangents[i][2] << "\n";
    }

    outFile.close();
}

void HermiteSpline::initializeCatmullRom() {
    tangents.clear();
    int n = points.size();

    if (n < 2) {
        // 控制点数量不足，无法计算切线
        return;
    }

    for (int i = 0; i < n; ++i) {
        std::array<double, 3> tangent;

        if (i == 0) {
            // 对第一个点使用二阶精确边界条件
            // P_{-1} = 2 * P_0 - P_1
            std::array<double, 3> P_minus1 = { 2.0 * points[0][0] - points[1][0],
                                               2.0 * points[0][1] - points[1][1],
                                               2.0 * points[0][2] - points[1][2] };
            tangent = { (points[1][0] - P_minus1[0]) * 0.5,
                        (points[1][1] - P_minus1[1]) * 0.5,
                        (points[1][2] - P_minus1[2]) * 0.5 };
        }
        else if (i == n - 1) {
            // 对最后一个点使用二阶精确边界条件
            // P_{n+1} = 2 * P_n - P_{n-1}
            std::array<double, 3> P_plus1 = { 2.0 * points[n - 1][0] - points[n - 2][0],
                                              2.0 * points[n - 1][1] - points[n - 2][1],
                                              2.0 * points[n - 1][2] - points[n - 2][2] };
            tangent = { (P_plus1[0] - points[n - 2][0]) * 0.5,
                        (P_plus1[1] - points[n - 2][1]) * 0.5,
                        (P_plus1[2] - points[n - 2][2]) * 0.5 };
        }
        else {
            // 对中间点使用 Catmull-Rom 公式
            tangent = { (points[i + 1][0] - points[i - 1][0]) * 0.5,
                        (points[i + 1][1] - points[i - 1][1]) * 0.5,
                        (points[i + 1][2] - points[i - 1][2]) * 0.5 };
        }

        tangents.push_back(tangent);  // 存储计算得到的切线
    }
}

void HermiteSpline::display(GLenum mode) {
    // Draw the curve as a series of line segments using old OpenGL
    glLineWidth(2.0f);
    glColor3f(0.3f, 0.7f, 0.1f);
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < points.size() - 1; ++i) {
        for (int j = 0; j < numSamples; ++j) {
            float t = static_cast<float>(j) / (numSamples - 1);
            std::array<double, 3> samplePoint;
            interpolate(static_cast<float>(i) + t, samplePoint.data());
            glVertex3f(samplePoint[0], samplePoint[1], samplePoint[2]);
        }
    }
    glEnd();

    // Draw control points using GLdrawSphere for better visualization
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const auto& point : points) {
        double center[3] = { point[0], point[1], point[2] };
        GLdrawSphere(0.1, center);
        /*
        glPointSize(0.05);
        glBegin(GL_POINTS);
        glVertex3dv(center);
        glEnd();
        */
    }
}

double HermiteSpline::getArcLength(double t) {
    // 通过 t 查找弧长表中的对应值
    int numSamples = arcLengthTable.size() - 1;
    float scaledT = t * numSamples;  // 缩放 t 到采样点的范围
    int index = static_cast<int>(scaledT);

    if (index >= numSamples) {
        return arcLengthTable[numSamples];  // 返回总弧长
    }

    // 在两个离散的 t 值之间进行线性插值
    float t1 = static_cast<float>(index) / numSamples;
    float t2 = static_cast<float>(index + 1) / numSamples;
    double arcLength1 = arcLengthTable[index];
    double arcLength2 = arcLengthTable[index + 1];

    // 线性插值计算
    double interpolatedLength = arcLength1 + (arcLength2 - arcLength1) * (t - t1) / (t2 - t1);
    return interpolatedLength;
}

int HermiteSpline::command(int argc, myCONST_SPEC char** argv) {
    
    if (argc < 1) {
        animTcl::OutputMessage("system %s: wrong number of params.", retrieveName().c_str());
        return TCL_ERROR;
    }
    else if (strcmp(argv[0], "set") == 0) {
        if (argc == 6 && strcmp(argv[1], "tangent") == 0) {
            int index = atoi(argv[2]);
            double tangent[3] = { atof(argv[3]), atof(argv[4]), atof(argv[5]) };
            setTangent(index, tangent);
            animTcl::OutputMessage("Set tangent at index %d to [%f, %f, %f]", index, tangent[0], tangent[1], tangent[2]);
            display();
            // glutPostRedisplay(); // 强制刷新窗口
            return TCL_OK;
        }
        else if (argc == 6 && strcmp(argv[1], "point") == 0) {
            int index = atoi(argv[2]);
            double point[3] = { atof(argv[3]), atof(argv[4]), atof(argv[5]) };
            setPoint(index, point);
            animTcl::OutputMessage("Set point at index %d to [%f, %f, %f]", index, point[0], point[1], point[2]);
            display();
            return TCL_OK;
        }
        else {
            animTcl::OutputMessage("Cannot conduct set actions.");
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[0], "add") == 0) {
        if (argc == 8 && strcmp(argv[1], "point") == 0) {
            if (points.size() >= 40) {
                animTcl::OutputMessage("The maximum controlPoints number is 40. Cannot add more points.");
                return TCL_ERROR;
            }

            double point[3] = { atof(argv[2]), atof(argv[3]), atof(argv[4]) };
            double tangent[3] = { atof(argv[5]), atof(argv[6]), atof(argv[7]) };
            addPoint(point, tangent);
            animTcl::OutputMessage("Added point [%f, %f, %f] with tangent [%f, %f, %f]",
                point[0], point[1], point[2], tangent[0], tangent[1], tangent[2]);
            display();
            return TCL_OK;
        }
        else {
            animTcl::OutputMessage("Cannot conduct add actions.");
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[0], "load") == 0 && argc == 2) {
        loadFromFile(argv[1]);
        display();
        // animTcl::OutputMessage("Loaded spline from file: %s", argv[1]);

        if (currentPart == 2) {
            // 设置 SampleParticle 的路径
            SampleParticle* porsche = dynamic_cast<SampleParticle*>(GlobalResourceManager::use()->getSystem("porsche"));
            if (porsche != nullptr) {
                porsche->setPath(this);
            }
            porsche->display();
        }
         return TCL_OK;
    }
    else if (strcmp(argv[0], "export") == 0 && argc == 2) {
        exportToFile(argv[1]);
        animTcl::OutputMessage("Exported spline to file: %s", argv[1]);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "cr") == 0 && argc == 1) {
        if (points.size() < 2)
        {
            animTcl::OutputMessage("Points less than 2. Can't conduct catmull-Rom Initialization.", argv[1]);
            return TCL_ERROR;
        }
        initializeCatmullRom();
        display();
        animTcl::OutputMessage("Have done the Catmull-Rom Initialization", argv[1]);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getArcLength") == 0 && argc == 2) {
        double t = atof(argv[1]);

        double arcLength = getArcLength(t);
        
        animTcl::OutputMessage("Arc Length up to t= %f is %f", t, arcLength );
        return TCL_OK;
    }

    animTcl::OutputMessage("system %s: unknown command.", retrieveName().c_str());
    return TCL_ERROR;
}