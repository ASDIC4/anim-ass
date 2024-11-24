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
    // glutPostRedisplay(); // ǿ��ˢ�´���
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

#include <glm/gtx/quaternion.hpp> // ��Ҫ�������ͷ�ļ���ʹ�� glm::quatLookAt

void SampleParticle::updatePosition(double t) {
    if (m_pathSpline) {
        double position[3];
        m_pathSpline->interpolate(t, position);
        m_position = glm::vec3(position[0], position[1], position[2]);

        double tangent[3];
        m_pathSpline->interpolate(t, tangent);
        glm::vec3 tangentVec(tangent[0], tangent[1], tangent[2]);

        // ʹ����Ԫ�����·���ʹ���������߶���
        glm::vec3 forward = glm::vec3(1.0f, 0.0f, 0.0f); // ԭʼǰ������
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

    // �������壬ʹ����
    glScaled(50.0, 50.0, 50.0);  // �� x, y, z ���������Ͻ�����Ŵ� 5 ��

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
            //// ���� HermiteSimulator ģ���������� SampleParticle �� HermiteSpline �ƶ�
            //HermiteSimulator* hermiteSimulator = new HermiteSimulator("hermiteSim", this, m_pathSpline);
            //bool success = GlobalResourceManager::use()->addSimulator(hermiteSimulator, true);
            //assert(success);
            //animTcl::OutputMessage("Started simulation.");
            display();
            if (!m_simulatorActive) {
                // ���� HermiteSimulator ģ���������� SampleParticle �� HermiteSpline �ƶ�
                HermiteSimulator* hermiteSimulator = new HermiteSimulator("hermiteSim", this, m_pathSpline);
                bool success = GlobalResourceManager::use()->addSimulator(hermiteSimulator, true);
                assert(success);
                m_simulatorActive = true; // ����״̬�����ģ����������
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
