#ifndef SAMPLE_PARTICLE_H
#define SAMPLE_PARTICLE_H
#define GLM_ENABLE_EXPERIMENTAL

#include "BaseSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLmodel/GLmodel.h>

class HermiteSpline; // ǰ������

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
    HermiteSpline* m_pathSpline; // ����·��
    glm::vec3 m_position;        // ��ǰλ����Ϣ
    glm::quat m_orientation;     // ��ǰ��̬��Ϣ
    double m_currentTime;
    bool m_simulatorActive; // ���ڸ���ģ�����Ƿ��Ѿ�����
};

#endif // SAMPLE_PARTICLE_H
