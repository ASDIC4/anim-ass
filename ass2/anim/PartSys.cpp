// PartSys.cpp
#include "PartSys.h"
#include "animTcl.h"
#include <GL/glut.h>

PartSys::PartSys(const std::string& name) : BaseSystem(name) {}

void PartSys::reset(double time)
{
    // Reset particles to initial positions and velocities
    for (auto& particle : particles) {
        particle.position = glm::vec3(0, 0, 0);
        particle.velocity = glm::vec3(0, 0, 0);
    }
}

void PartSys::getState(double* p)
{
    // 将粒子的状态复制到状态数组 p 中
    for (int i = 0; i < particles.size(); ++i) {
        int index = i * 6;
        p[index] = particles[i].position.x;
        p[index + 1] = particles[i].position.y;
        p[index + 2] = particles[i].position.z;
        p[index + 3] = particles[i].velocity.x;
        p[index + 4] = particles[i].velocity.y;
        p[index + 5] = particles[i].velocity.z;
    }
}

void PartSys::setState(double* p)
{
    // 从状态数组 p 中读取粒子状态
    for (int i = 0; i < particles.size(); ++i) {
        int index = i * 6;
        particles[i].position = glm::vec3(p[index], p[index + 1], p[index + 2]);
        particles[i].velocity = glm::vec3(p[index + 3], p[index + 4], p[index + 5]);
    }
}

int PartSys::command(int argc, myCONST_SPEC char** argv)
{   
    /*animTcl::OutputMessage("output full sentence: ", m_name.c_str());*/

    std::string fullCommand = "Command received: ";
    for (int i = 0; i < argc; ++i) {
        fullCommand += argv[i];
        if (i < argc - 1) fullCommand += " ";
    }
    animTcl::OutputMessage("%s", fullCommand.c_str());

    if (argc < 1)
    {
        animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
        return TCL_ERROR;
    }
    // 1. system <sys_name> dim <Number of Particles>
    else if (strcmp(argv[0], "dim") == 0)
    {
        // 确保有两个参数，第一个是 "dim"，第二个是粒子数量
        if (argc != 2)
        {
            animTcl::OutputMessage("Usage: system %s dim <Number of Particles>", m_name.c_str());
            return TCL_ERROR;
        }
        // 解析粒子数量参数
        int numParticles = atoi(argv[1]);
        if (numParticles <= 0)
        {
            animTcl::OutputMessage("Error: Number of particles must be positive.");
            return TCL_ERROR;
        }
        // 清空当前粒子数组并重新分配
        particles.clear();
        particles.resize(numParticles);

        // 初始化粒子，默认位置为原点，速度为零，质量为 1.0，未固定
        for (int i = 0; i < numParticles; ++i)
        {
            Particle particle;
            particle.position = glm::vec3(0.0f);
            particle.velocity = glm::vec3(0.0f);
            particle.mass = 1.0f;
            particle.fixed = false;
            particles[i] = particle;
        }

        animTcl::OutputMessage("Particle system initialized with %d particles.", numParticles);
        return TCL_OK;
    }
    // 2. system <sys_name> particle <index> <mass> <x y z vx vy vz> 
    else if (strcmp(argv[0], "particle") == 0) {
        // 确保参数数量正确
        if (argc != 9) {
            animTcl::OutputMessage("Usage: system %s particle <index> <mass> <x y z vx vy vz>", m_name.c_str());
            return TCL_ERROR;
        }
        // 解析粒子索引并检查有效性
        int index = atoi(argv[1]);
        if (index < 0 || index >= particles.size()) {
            animTcl::OutputMessage("Error: Particle index %d is out of bounds.", index);
            return TCL_ERROR;
        }
        // 解析质量并检查有效性
        float mass = atof(argv[2]);
        if (mass <= 0) {
            animTcl::OutputMessage("Error: Particle mass must be positive.");
            return TCL_ERROR;
        }

        // 解析位置和速度
        glm::vec3 position(atof(argv[3]), atof(argv[4]), atof(argv[5]));
        glm::vec3 velocity(atof(argv[6]), atof(argv[7]), atof(argv[8]));

        // 设置粒子位置、速度、质量，默认未固定
        particles[index] = { position, velocity, mass, false };

        animTcl::OutputMessage("Particle %d set with mass %.2f, position (%.2f, %.2f, %.2f), and velocity (%.2f, %.2f, %.2f).",
            index, mass, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
        return TCL_OK;
    }
    // 3. system <sys_name> all_velocities  <vx vy vz>
    // This command sets the velocity of all particles.
    else if (strcmp(argv[0], "all_velocities") == 0) {
        // 确保参数数量正确
        if (argc != 4) {
            animTcl::OutputMessage("Usage: system %s all_velocities <vx vy vz>", m_name.c_str());
            return TCL_ERROR;
        }

        // 解析速度值
        glm::vec3 velocity(atof(argv[1]), atof(argv[2]), atof(argv[3]));

        // 设置所有粒子的速度
        for (auto& particle : particles) {
            if (!particle.fixed) {  // 确保未固定的粒子才被更新
                particle.velocity = velocity;
            }
        }
        animTcl::OutputMessage("Set velocity of all particles to (%.2f, %.2f, %.2f).", velocity.x, velocity.y, velocity.z);
        return TCL_OK;
    }
    // 4. simulator <sim_name> link <sys name> <Number of Springs>
    //    This links the simulator to a particular particle system and initializes it to work with a given number of springs.
    else if (strcmp(argv[0], "link") == 0) {
        // 处理链接命令
        if (argc != 4) {
            animTcl::OutputMessage("Usage: system %s link <simulator name> <Number of Springs>", m_name.c_str());
            return TCL_ERROR;
        }

        // 解析弹簧数量并检查有效性
        int numSprings = atoi(argv[3]);
        if (numSprings <= 0) {
            animTcl::OutputMessage("Error: Number of springs must be positive.");
            return TCL_ERROR;
        }

        animTcl::OutputMessage("System %s linked to simulator %s with %d springs.", m_name.c_str(), argv[1], numSprings);
        return TCL_OK;
    }
    //else if (strcmp(argv[0], "spring") == 0) {
    //    // 添加弹簧
    //    int p1 = atoi(argv[1]);
    //    int p2 = atoi(argv[2]);
    //    float ks = atof(argv[3]);
    //    float kd = atof(argv[4]);
    //    float restLength = atof(argv[5]);
    //    addSpring(p1, p2, ks, kd, restLength);
    //    return TCL_OK;
    //}
    return TCL_ERROR;
}

void PartSys::addSpring(int p1, int p2, float ks, float kd, float restLength)
{
    springs.push_back({ p1, p2, ks, kd, restLength });
}



void PartSys::display(GLenum mode)
{
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 设置摄像机视角，从右上方俯视立方体
    gluLookAt(1.5, 0.5, 1.0,   // 摄像机位置 (可以调整数值来改变视角)
        0.0, 0.0, 0.0,   // 摄像机注视点
        0.0, 1.0, 0.0);  // 摄像机“上”方向

    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // 缩放模型视图矩阵，将整个系统缩小为原来的50%
    glScalef(0.5f, 0.5f, 0.5f);

    // 绘制粒子为绿色点或球体
    if (particles.size() < 50) { // 当粒子数量少于50时，绘制球体
        for (const auto& particle : particles)
        {
            if (particle.fixed || particle.position.y < 0.0f) continue;  // 跳过固定的粒子和低于地面的粒子

            glPushMatrix();
            glTranslatef(particle.position.x, particle.position.y, particle.position.z);
            glColor3f(0.0f, 1.0f, 0.0f); // 设置粒子的颜色为绿色
            glutSolidSphere(0.12, 20, 20); // 绘制小球
            glPopMatrix();
        }
    }
    else { // 当粒子数量较多时，使用GL_POINTS绘制
        glPointSize(5.0f); // 设置点的大小
        glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 0.0f); // 设置粒子的颜色为绿色
        for (const auto& particle : particles)
        {
            if (particle.fixed || particle.position.y < 0.0f) continue;  // 跳过固定的粒子和低于地面的粒子
            glVertex3f(particle.position.x, particle.position.y, particle.position.z);
        }
        glEnd();
    }

    // 绘制粒子之间的弹簧为深黄色线条
    glColor3f(0.5f, 0.5f, 0.0f); // 设置弹簧的颜色为深黄色
    for (const auto& spring : springs)
    {
        const Particle& p1 = particles[spring.p1];
        const Particle& p2 = particles[spring.p2];

        // 如果两个粒子都在地面以下，则跳过该弹簧
        if (p1.position.y < 0.0f && p2.position.y < 0.0f) continue;

        // 分段绘制弹簧，只绘制位于y >= 0的部分
        if (p1.position.y >= 0.0f && p2.position.y >= 0.0f) {
            // 如果两个粒子都在地面以上，完整绘制弹簧
            glBegin(GL_LINES);
            glVertex3f(p1.position.x, p1.position.y, p1.position.z);
            glVertex3f(p2.position.x, p2.position.y, p2.position.z);
            glEnd();
        }
        else {
            // 一个粒子在y >= 0，另一个在y < 0，计算截断点
            glm::vec3 start = p1.position.y >= 0.0f ? p1.position : p2.position;
            glm::vec3 end = p1.position.y < 0.0f ? p1.position : p2.position;

            // 计算在y=0平面上的交点
            float t = start.y / (start.y - end.y);
            glm::vec3 intersection = start + t * (end - start);

            glBegin(GL_LINES);
            glVertex3f(start.x, start.y, start.z);
            glVertex3f(intersection.x, intersection.y, intersection.z);
            glEnd();
        }
    }

    glPopAttrib();
    glPopMatrix();
}