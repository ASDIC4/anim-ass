//// PartSys.h
//#pragma once
//#ifndef PART_SYS_H
//#define PART_SYS_H
//
//#include "BaseSystem.h"
//#include <vector>
//#include <glm/glm.hpp>
//
//struct Particle {
//    glm::vec3 position;
//    glm::vec3 velocity;
//    float mass;
//    bool fixed;
//};
//
//struct Spring {
//    int p1, p2;
//    float ks;    // 弹性系数
//    float kd;    // 阻尼系数
//    float restLength;
//};
//
//class PartSys : public BaseSystem
//{
//public:
//    PartSys(const std::string& name);
//    void reset(double time);
//    void getState(double* p);
//    void setState(double* p);
//    int command(int argc, myCONST_SPEC char** argv);
//    void addSpring(int p1, int p2, float ks, float kd, float restLength);
//
//private:
//    std::vector<Particle> particles;
//    std::vector<Spring> springs;
//};
//
//#endif


// PartSys.h
#pragma once
#ifndef PART_SYS_H
#define PART_SYS_H

#include "BaseSystem.h"
#include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    bool fixed;
};

struct Spring {
    int p1, p2;
    float ks;    // 弹性系数
    float kd;    // 阻尼系数
    float restLength;
};

class PartSys : public BaseSystem
{
public:
    PartSys(const std::string& name);
    virtual void getState(double* p);
    virtual void setState(double* p);

    void display(GLenum mode = GL_RENDER);

    void reset(double time);
    int command(int argc, myCONST_SPEC char** argv);
    void addSpring(int p1, int p2, float ks, float kd, float restLength);

    // 新增的访问方法
    const std::vector<Spring>& getSprings() const { return springs; }
    std::vector<Particle>& getParticles() { return particles; }

private:
    std::vector<Particle> particles;
    std::vector<Spring> springs;
};

#endif
