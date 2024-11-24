#define GLM_ENABLE_EXPERIMENTAL
#include "SampleParticle.h"
#include "HermiteSpline.h"
#include <glm/gtc/quaternion.hpp>
#include <GL/glut.h>
#include "HermiteSimulator.h"
#include "GlobalResourceManager.h"

SampleParticle::SampleParticle(const std::string& name)
    : BaseSystem(name), m_pathSpline(nullptr), m_currentTime(0.0), m_simulatorActive(false) {}

void SampleParticle::loadModel(const std::string& filename) {
    m_model.ReadOBJ(filename.c_str());
    glmFacetNormals(&m_model);
    glmVertexNormals(&m_model, 90);

    /*if (m_model.numvertices > 0) {
        animTcl::OutputMessage("Loaded model from file: %s with %d vertices", filename.c_str(), m_model.numvertices);
    }
    else {
        animTcl::OutputMessage("Failed to load model or model has no vertices: %s", filename.c_str());
    }*/
    // glutPostRedisplay(); // 强制刷新窗口
}

void SampleParticle::setPath(HermiteSpline* spline) {
    m_pathSpline = spline;
    if (spline) {
        double initialPosition[3];
        spline->interpolate(0.0f, initialPosition);
        m_position = glm::vec3(initialPosition[0], initialPosition[1], initialPosition[2]);
    }
}

void SampleParticle::reset(double time) {
    m_currentTime = 0.0;
    m_simulatorActive = false;
    if (m_pathSpline) {
        double initialPosition[3];
        m_pathSpline->interpolate(0.0f, initialPosition);
        m_position = glm::vec3(initialPosition[0], initialPosition[1], initialPosition[2]);
    }
}

void SampleParticle::getState(double* p) {
    p[0] = m_position.x;
    p[1] = m_position.y;
    p[2] = m_position.z;
}

void SampleParticle::setState(double* p) {
    m_position = glm::vec3(p[0], p[1], p[2]);
}

#include <glm/gtx/quaternion.hpp> // 需要包含这个头文件以使用 glm::quatLookAt

void SampleParticle::updatePosition(double t) {
    if (m_pathSpline) {
        double position[3];
        m_pathSpline->interpolate(t, position);
        m_position = glm::vec3(position[0], position[1], position[2]);

        double tangent[3];
        m_pathSpline->interpolate(t, tangent);
        glm::vec3 tangentVec(tangent[0], tangent[1], tangent[2]);

        // 使用四元数更新方向，使物体沿切线对齐
        glm::vec3 forward = glm::vec3(1.0f, 0.0f, 0.0f); // 原始前进方向
        m_orientation = glm::quatLookAt(glm::normalize(tangentVec), glm::vec3(0.0f, 1.0f, 0.0f));
    }
}
#include <glm/gtc/type_ptr.hpp>


void SampleParticle::display(GLenum mode) {

    animTcl::OutputMessage("Displaying SampleParticle: %s", retrieveName().c_str());

    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glTranslated(m_position.x, m_position.y, m_position.z);
    animTcl::OutputMessage("X Y Z : %f , %f, %f", 
        m_position.x, m_position.y, m_position.z);
    glMultMatrixf(glm::value_ptr(glm::mat4_cast(m_orientation)));

    // 缩放物体，使其变大
    glScaled(50.0, 50.0, 50.0);  // 在 x, y, z 三个方向上将物体放大 5 倍

    if (m_model.numvertices > 0) {
        animTcl::OutputMessage("RIGHT");
        glmDraw(&m_model, GLM_SMOOTH | GLM_MATERIAL);
    }
    glPopMatrix();
    glPopAttrib();
}

int SampleParticle::command(int argc, myCONST_SPEC char** argv) {
    if (argc < 1) {
        animTcl::OutputMessage("system %s: wrong number of params.", retrieveName().c_str());
        return TCL_ERROR;
    }
    else if (strcmp(argv[0], "load") == 0 && argc == 2) {
        loadModel(argv[1]);
        display();
        animTcl::OutputMessage("Loaded model from file: %s", argv[1]);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "reset") == 0 && argc == 1) {
        reset(0.0);
        display();
        animTcl::OutputMessage("Reset particle to initial position.");
        return TCL_OK;
    }
    else if (strcmp(argv[0], "start") == 0 && argc == 1) {
        if (m_pathSpline != nullptr) {
            //// 创建 HermiteSimulator 模拟器，控制 SampleParticle 沿 HermiteSpline 移动
            //HermiteSimulator* hermiteSimulator = new HermiteSimulator("hermiteSim", this, m_pathSpline);
            //bool success = GlobalResourceManager::use()->addSimulator(hermiteSimulator, true);
            //assert(success);
            //animTcl::OutputMessage("Started simulation.");
            display();
            if (!m_simulatorActive) {
                // 创建 HermiteSimulator 模拟器，控制 SampleParticle 沿 HermiteSpline 移动
                HermiteSimulator* hermiteSimulator = new HermiteSimulator("hermiteSim", this, m_pathSpline);
                bool success = GlobalResourceManager::use()->addSimulator(hermiteSimulator, true);
                assert(success);
                m_simulatorActive = true; // 更新状态，标记模拟器已启动
                animTcl::OutputMessage("Started simulation.");
            }
            else {
                animTcl::OutputMessage("Simulation already started.");
            }
        }
        else {
            animTcl::OutputMessage("Path spline not loaded.");
        }
        return TCL_OK;
    }

    animTcl::OutputMessage("system %s: unknown command.", retrieveName().c_str());
    return TCL_ERROR;
}
