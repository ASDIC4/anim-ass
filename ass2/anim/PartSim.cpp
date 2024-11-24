// PartSim.cpp
#include "PartSys.h"
#include "PartSim.h"
#define GLM_ENABLE_EXPERIMENTAL  // 必要时启用实验性功能支持
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp> // 用于 length2 快速计算平方长度
#include "animTcl.h"

PartSim::PartSim(const std::string& name, BaseSystem* target) :
    BaseSimulator(name),
    m_system(dynamic_cast<PartSys*>(target)),
    m_integrationMethod(EULER),
    m_gravity(glm::vec3(0, -9.8f, 0)),
    m_drag(0.0f),
    numSprings(0),
    groundKs(0.0f),
    groundKd(0.0f)
{}

int PartSim::step(double time) {
    double dt = m_timeStep;
    //    time - prev_time;
    prev_time = time;
   
    animTcl::OutputMessage("Now is the dt %lf", dt);

    // 获取粒子系统的粒子数组和状态数组大小
    // get the particle array size
    int numParticles = m_system->getParticles().size();
    int stateSize = numParticles * 6; 
    // 假设每个粒子状态包含3位置 + 3速度 = 6个属性
    // states includes 3 positions + 3 velocities = 6 properties

    // 获取粒子系统状态到临时数组 particleState
    // get the temp state to particleState this array
    double* particleState = new double[stateSize];
    m_system->getState(particleState);

    // 存储所有粒子的合力
    // store all the forces of particles
    std::vector<glm::vec3> forces(numParticles, glm::vec3(0.0f));

    // 计算每对粒子的弹簧力和阻尼力
    // calculate teh spring force and damping force
    for (const auto& spring : m_system->getSprings()) {
        int p1Index = spring.p1 * 6;
        int p2Index = spring.p2 * 6;

        // 获取两端粒子的位置
        // get the positions of particles at two ends
        glm::vec3 p1Pos(particleState[p1Index], particleState[p1Index + 1], particleState[p1Index + 2]);
        glm::vec3 p2Pos(particleState[p2Index], particleState[p2Index + 1], particleState[p2Index + 2]);

        // 计算弹簧力
        // calculate the spring force
        glm::vec3 dir = p2Pos - p1Pos; // P1 -> P2 vector include length
        float len = glm::length(dir);
        glm::vec3 springForce = spring.ks * (len - spring.restLength) * (dir / len);

        // 获取两端粒子速度
        // get velocities of particles at two ends
        glm::vec3 p1Vel(particleState[p1Index + 3], particleState[p1Index + 4], particleState[p1Index + 5]);
        glm::vec3 p2Vel(particleState[p2Index + 3], particleState[p2Index + 4], particleState[p2Index + 5]);

        // 计算阻尼力
        // calculate the damping force
        glm::vec3 relativeVel = p2Vel - p1Vel;
        glm::vec3 dampingForce = spring.kd * relativeVel;

        // 将合力添加到粒子上
        // add all forces to particles
        if (!m_system->getParticles()[spring.p1].fixed)
            forces[spring.p1] += springForce - dampingForce;
        if (!m_system->getParticles()[spring.p2].fixed)
            forces[spring.p2] -= springForce - dampingForce;
    }

    // 对每个粒子施加重力、阻力，并应用地面碰撞惩罚
    // apply gravity, damping force, and apply ground collision penalty 
    for (int i = 0; i < numParticles; ++i) {
        int index = i * 6;

        if (m_system->getParticles()[i].fixed) continue;

        // 重力
        // gravity
        forces[i] += m_system->getParticles()[i].mass * m_gravity;

        // 阻力
        // damping force
        glm::vec3 velocity(particleState[index + 3], particleState[index + 4], particleState[index + 5]);
        forces[i] -= m_drag * velocity;

        // 地面碰撞惩罚（假设地面在 y=0 位置）
        // ground collision penalty ( assume groud at y = 0 level )
        // 地面碰撞惩罚力和阻尼力（修改处）
        //glm::vec3 position(particleState[index], particleState[index + 1], particleState[index + 2]);
        //if (position.y < 0.0f) {  // 当粒子低于地面时，施加惩罚力和阻尼力
        //    glm::vec3 groundForce(0, -groundKs * position.y, 0);  // 惩罚力与地面距离成正比
        //    glm::vec3 groundDamping(0, -groundKd * velocity.y, 0);  // 阻尼力与速度成正比
        //    forces[i] += groundForce + groundDamping;
        //}
        glm::vec3 position(particleState[index], particleState[index + 1], particleState[index + 2]);
        if (position.y < 0.0f) {
            // 计算惩罚力 F^SP
            glm::vec3 groundForce = -groundKs * position - groundKd * velocity;

            // 计算法向力 F^N
            glm::vec3 normal = glm::vec3(0, 1, 0); // 假设地面法向量为 (0, 1, 0)
            glm::vec3 normalForce = glm::dot(groundForce, normal) * normal;

            // 计算切向力 F_tangent
            glm::vec3 tangentForce = groundForce - normalForce;

            // 摩擦锥裁剪
            float tangentMagnitude = glm::length(tangentForce);
            float maxTangent = m_drag * glm::length(normalForce); // mu 是摩擦系数
            if (tangentMagnitude > maxTangent) {
                tangentForce = (tangentForce / tangentMagnitude) * maxTangent;
            }

            // 总地面惩罚力 = 法向力 + 裁剪后的切向力
            forces[i] += normalForce + tangentForce;
        }
    }

    // 根据积分方法更新粒子位置和速度
    // by diff intergration methods
    switch (m_integrationMethod) {
    case EULER:
        // 欧拉积分 Euler Intergration 
        for (int i = 0; i < numParticles; ++i) {
            int index = i * 6;
            if (m_system->getParticles()[i].fixed) continue;

            glm::vec3 acceleration = forces[i] / m_system->getParticles()[i].mass;            

            // 更新位置 update positions
            particleState[index] += particleState[index + 3] * dt;
            particleState[index + 1] += particleState[index + 4] * dt;
            particleState[index + 2] += particleState[index + 5] * dt;

            // 更新速度 update velocity
            particleState[index + 3] += acceleration.x * dt;
            particleState[index + 4] += acceleration.y * dt;
            particleState[index + 5] += acceleration.z * dt;
        }
        break;

    case SYMPLECTIC_EULER:
        // Symplectic Euler Intergration 积分
        for (int i = 0; i < numParticles; ++i) {
            int index = i * 6;
            if (m_system->getParticles()[i].fixed) continue;

            // 更新速度 update velocity
            glm::vec3 acceleration = forces[i] / m_system->getParticles()[i].mass;
            particleState[index + 3] += acceleration.x * dt;
            particleState[index + 4] += acceleration.y * dt;
            particleState[index + 5] += acceleration.z * dt;

            // 更新位置 update positions
            particleState[index] += particleState[index + 3] * dt;
            particleState[index + 1] += particleState[index + 4] * dt;
            particleState[index + 2] += particleState[index + 5] * dt;
        }
        break;

    case VERLET:
        // Verlet 积分 Verlet Intergration
        if (prev_positions.empty()) {
            prev_positions.resize(numParticles, glm::vec3(0.0f));
            for (int i = 0; i < numParticles; ++i) {
                int index = i * 6;
                prev_positions[i] = glm::vec3(particleState[index], particleState[index + 1], particleState[index + 2]);
            }
        }
        for (int i = 0; i < numParticles; ++i) {
            int index = i * 6;
            if (m_system->getParticles()[i].fixed) continue;

            glm::vec3 currentPosition(particleState[index], particleState[index + 1], particleState[index + 2]);
            glm::vec3 acceleration = forces[i] / m_system->getParticles()[i].mass;
            glm::vec3 newPosition = 2.0f * currentPosition - prev_positions[i] + acceleration * (float)(dt * dt);

            prev_positions[i] = currentPosition;
            particleState[index] = newPosition.x;
            particleState[index + 1] = newPosition.y;
            particleState[index + 2] = newPosition.z;
        }
        break;
    }

    // 将更新后的状态写回粒子系统
    // write back the updated state of particles
    m_system->setState(particleState);
    delete[] particleState;

    glutPostRedisplay();  // 刷新显示 refresh display
    return 0;
}


int PartSim::command(int argc, myCONST_SPEC char** argv)
{   
    /*std::string fullCommand = "Command received: ";
    for (int i = 0; i < argc; ++i) {
        fullCommand += argv[i];
        if (i < argc - 1) fullCommand += " ";
    }
    animTcl::OutputMessage("%s", fullCommand.c_str());*/


    // 4. simulator <sim_name> link <sys name> <Number of Springs>
    if (strcmp(argv[0], "link") == 0) {
        if (argc != 4) {
            animTcl::OutputMessage("Usage: simulator %s link <sys name> <Number of Springs>", m_name.c_str());
            return TCL_ERROR;
        }

        numSprings = atoi(argv[3]);
        if (numSprings <= 0) {
            animTcl::OutputMessage("Error: Number of springs must be positive.");
            return TCL_ERROR;
        }

        animTcl::OutputMessage("Simulator %s linked to system %s with %d springs.", m_name.c_str(), argv[1], numSprings);
        return TCL_OK;
    }

    // 5. simulator <sim_name> spring <index1> <index2> <ks> <kd> <restlength>
    else if (strcmp(argv[0], "spring") == 0) {
        if (argc != 6) {
            animTcl::OutputMessage("Usage: simulator %s spring <index1> <index2> <ks> <kd> <restlength>", m_name.c_str());
            return TCL_ERROR;
        }

        int index1 = atoi(argv[1]);
        int index2 = atoi(argv[2]);
        float ks = atof(argv[3]);
        float kd = atof(argv[4]);
        float restLength = atof(argv[5]);

        m_system->addSpring(index1, index2, ks, kd, restLength);
        animTcl::OutputMessage("Spring added between particles %d and %d with ks=%.2f, kd=%.2f, restLength=%.2f.", index1, index2, ks, kd, restLength);
        return TCL_OK;
    }
    // 6. simulator <sim_name> fix <index>
    else if (strcmp(argv[0], "fix") == 0) {
        if (argc != 2) {
            animTcl::OutputMessage("Usage: simulator %s fix <index>", m_name.c_str());
            return TCL_ERROR;
        }
        int index = atoi(argv[1]);
        if (index < 0 || index >= m_system->getParticles().size()) {
            animTcl::OutputMessage("Error: Particle index %d is out of bounds.", index);
            return TCL_ERROR;
        }
        m_system->getParticles()[index].fixed = true;
        animTcl::OutputMessage("Particle %d is now fixed in place.", index);
        return TCL_OK;
    }
    // 7. simulator <sim_name> integration <euler|symplectic|verlet> <time step>
    else if (strcmp(argv[0], "integration") == 0) {
        if (argc != 3) { // 检查是否提供了正确的参数数量
            animTcl::OutputMessage("Usage: simulator %s integration <euler|symplectic|verlet> <time step>", m_name.c_str());
            return TCL_ERROR;
        }

        // 设置积分方法
        // set intergration method
        if (strcmp(argv[1], "euler") == 0) m_integrationMethod = EULER;
        else if (strcmp(argv[1], "symplectic") == 0) m_integrationMethod = SYMPLECTIC_EULER;
        else if (strcmp(argv[1], "verlet") == 0) m_integrationMethod = VERLET;
        else {
            animTcl::OutputMessage("Unknown integration method: %s", argv[1]);
            return TCL_ERROR;
        }

        // 设置时间步长
        // set timestep
        m_timeStep = atof(argv[2]);
        animTcl::OutputMessage("Integration method set to %s with time step %f", argv[1], m_timeStep);
        return TCL_OK;
    }
    // 8. simulator <sim_name> ground <ks> <kd>
    else if (strcmp(argv[0], "ground") == 0) {
        if (argc != 3) {
            animTcl::OutputMessage("Usage: simulator %s ground <ks> <kd>", m_name.c_str());
            return TCL_ERROR;
        }

        groundKs = atof(argv[1]);
        groundKd = atof(argv[2]);
        animTcl::OutputMessage("Ground collision penalty set with ks=%.2f, kd=%.2f.", groundKs, groundKd);
        return TCL_OK;
    }
    // 9. simulator <sim_name> gravity <g>
    else if (strcmp(argv[0], "gravity") == 0) {
        if (argc != 2) {
            animTcl::OutputMessage("Usage: simulator %s gravity <g>", m_name.c_str());
            return TCL_ERROR;
        }

        float gravity = atof(argv[1]);
        m_gravity = glm::vec3(0, gravity, 0);
        animTcl::OutputMessage("Gravity set to %.2f.", gravity);
        return TCL_OK;
    }

    // 10. simulator <sim_name> drag <kdrag>
    else if (strcmp(argv[0], "drag") == 0) {
        if (argc != 2) {
            animTcl::OutputMessage("Usage: simulator %s drag <kdrag>", m_name.c_str());
            return TCL_ERROR;
        }

        m_drag = atof(argv[1]);
        animTcl::OutputMessage("Drag coefficient set to %.2f.", m_drag);
        return TCL_OK;
    }
    return TCL_ERROR;
}

void PartSim::reset(double time) {
    // 重置系统
    prev_time = 0;
}