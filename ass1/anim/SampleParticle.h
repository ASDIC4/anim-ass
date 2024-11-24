#ifndef SAMPLE_PARTICLE_H
#define SAMPLE_PARTICLE_H
#define GLM_ENABLE_EXPERIMENTAL

#include "BaseSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLmodel/GLmodel.h>

class HermiteSpline; // 前向声明

class SampleParticle : public BaseSystem {
public:
    SampleParticle(const std::string& name);

    void loadModel(const std::string& filename);
    void setPath(HermiteSpline* spline);
    void reset(double time);
    void getState(double* p) override;
    void setState(double* p) override;
    void updatePosition(double t);
    void display(GLenum mode = GL_RENDER) override;
    int command(int argc, myCONST_SPEC char** argv) override;

private:
    GLMmodel m_model;
    HermiteSpline* m_pathSpline; // 样条路径
    glm::vec3 m_position;        // 当前位置信息
    glm::quat m_orientation;     // 当前姿态信息
    double m_currentTime;
    bool m_simulatorActive; // 用于跟踪模拟器是否已经启动
};

#endif // SAMPLE_PARTICLE_H
