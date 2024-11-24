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
    // �����ӵ�״̬���Ƶ�״̬���� p ��
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
    // ��״̬���� p �ж�ȡ����״̬
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
        // ȷ����������������һ���� "dim"���ڶ�������������
        if (argc != 2)
        {
            animTcl::OutputMessage("Usage: system %s dim <Number of Particles>", m_name.c_str());
            return TCL_ERROR;
        }
        // ����������������
        int numParticles = atoi(argv[1]);
        if (numParticles <= 0)
        {
            animTcl::OutputMessage("Error: Number of particles must be positive.");
            return TCL_ERROR;
        }
        // ��յ�ǰ�������鲢���·���
        particles.clear();
        particles.resize(numParticles);

        // ��ʼ�����ӣ�Ĭ��λ��Ϊԭ�㣬�ٶ�Ϊ�㣬����Ϊ 1.0��δ�̶�
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
        // ȷ������������ȷ
        if (argc != 9) {
            animTcl::OutputMessage("Usage: system %s particle <index> <mass> <x y z vx vy vz>", m_name.c_str());
            return TCL_ERROR;
        }
        // �������������������Ч��
        int index = atoi(argv[1]);
        if (index < 0 || index >= particles.size()) {
            animTcl::OutputMessage("Error: Particle index %d is out of bounds.", index);
            return TCL_ERROR;
        }
        // ���������������Ч��
        float mass = atof(argv[2]);
        if (mass <= 0) {
            animTcl::OutputMessage("Error: Particle mass must be positive.");
            return TCL_ERROR;
        }

        // ����λ�ú��ٶ�
        glm::vec3 position(atof(argv[3]), atof(argv[4]), atof(argv[5]));
        glm::vec3 velocity(atof(argv[6]), atof(argv[7]), atof(argv[8]));

        // ��������λ�á��ٶȡ�������Ĭ��δ�̶�
        particles[index] = { position, velocity, mass, false };

        animTcl::OutputMessage("Particle %d set with mass %.2f, position (%.2f, %.2f, %.2f), and velocity (%.2f, %.2f, %.2f).",
            index, mass, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
        return TCL_OK;
    }
    // 3. system <sys_name> all_velocities  <vx vy vz>
    // This command sets the velocity of all particles.
    else if (strcmp(argv[0], "all_velocities") == 0) {
        // ȷ������������ȷ
        if (argc != 4) {
            animTcl::OutputMessage("Usage: system %s all_velocities <vx vy vz>", m_name.c_str());
            return TCL_ERROR;
        }

        // �����ٶ�ֵ
        glm::vec3 velocity(atof(argv[1]), atof(argv[2]), atof(argv[3]));

        // �����������ӵ��ٶ�
        for (auto& particle : particles) {
            if (!particle.fixed) {  // ȷ��δ�̶������Ӳű�����
                particle.velocity = velocity;
            }
        }
        animTcl::OutputMessage("Set velocity of all particles to (%.2f, %.2f, %.2f).", velocity.x, velocity.y, velocity.z);
        return TCL_OK;
    }
    // 4. simulator <sim_name> link <sys name> <Number of Springs>
    //    This links the simulator to a particular particle system and initializes it to work with a given number of springs.
    else if (strcmp(argv[0], "link") == 0) {
        // ������������
        if (argc != 4) {
            animTcl::OutputMessage("Usage: system %s link <simulator name> <Number of Springs>", m_name.c_str());
            return TCL_ERROR;
        }

        // �������������������Ч��
        int numSprings = atoi(argv[3]);
        if (numSprings <= 0) {
            animTcl::OutputMessage("Error: Number of springs must be positive.");
            return TCL_ERROR;
        }

        animTcl::OutputMessage("System %s linked to simulator %s with %d springs.", m_name.c_str(), argv[1], numSprings);
        return TCL_OK;
    }
    //else if (strcmp(argv[0], "spring") == 0) {
    //    // ��ӵ���
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

    // ����������ӽǣ������Ϸ�����������
    gluLookAt(1.5, 0.5, 1.0,   // �����λ�� (���Ե�����ֵ���ı��ӽ�)
        0.0, 0.0, 0.0,   // �����ע�ӵ�
        0.0, 1.0, 0.0);  // ��������ϡ�����

    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // ����ģ����ͼ���󣬽�����ϵͳ��СΪԭ����50%
    glScalef(0.5f, 0.5f, 0.5f);

    // ��������Ϊ��ɫ�������
    if (particles.size() < 50) { // ��������������50ʱ����������
        for (const auto& particle : particles)
        {
            if (particle.fixed || particle.position.y < 0.0f) continue;  // �����̶������Ӻ͵��ڵ��������

            glPushMatrix();
            glTranslatef(particle.position.x, particle.position.y, particle.position.z);
            glColor3f(0.0f, 1.0f, 0.0f); // �������ӵ���ɫΪ��ɫ
            glutSolidSphere(0.12, 20, 20); // ����С��
            glPopMatrix();
        }
    }
    else { // �����������϶�ʱ��ʹ��GL_POINTS����
        glPointSize(5.0f); // ���õ�Ĵ�С
        glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 0.0f); // �������ӵ���ɫΪ��ɫ
        for (const auto& particle : particles)
        {
            if (particle.fixed || particle.position.y < 0.0f) continue;  // �����̶������Ӻ͵��ڵ��������
            glVertex3f(particle.position.x, particle.position.y, particle.position.z);
        }
        glEnd();
    }

    // ��������֮��ĵ���Ϊ���ɫ����
    glColor3f(0.5f, 0.5f, 0.0f); // ���õ��ɵ���ɫΪ���ɫ
    for (const auto& spring : springs)
    {
        const Particle& p1 = particles[spring.p1];
        const Particle& p2 = particles[spring.p2];

        // ����������Ӷ��ڵ������£��������õ���
        if (p1.position.y < 0.0f && p2.position.y < 0.0f) continue;

        // �ֶλ��Ƶ��ɣ�ֻ����λ��y >= 0�Ĳ���
        if (p1.position.y >= 0.0f && p2.position.y >= 0.0f) {
            // ����������Ӷ��ڵ������ϣ��������Ƶ���
            glBegin(GL_LINES);
            glVertex3f(p1.position.x, p1.position.y, p1.position.z);
            glVertex3f(p2.position.x, p2.position.y, p2.position.z);
            glEnd();
        }
        else {
            // һ��������y >= 0����һ����y < 0������ضϵ�
            glm::vec3 start = p1.position.y >= 0.0f ? p1.position : p2.position;
            glm::vec3 end = p1.position.y < 0.0f ? p1.position : p2.position;

            // ������y=0ƽ���ϵĽ���
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