#pragma once
// PartSim.h
#pragma once
#ifndef PART_SIM_H
#define PART_SIM_H

#include "BaseSimulator.h"
#include "PartSys.h"

enum IntegrationMethod {
    EULER,
    SYMPLECTIC_EULER,
    VERLET
};

class PartSim : public BaseSimulator
{
public:
    PartSim(const std::string& name, BaseSystem* target);
    int step(double time);
    void reset(double time);
    int command(int argc, myCONST_SPEC char** argv);
    int init(double time)
    {
        return 0;
    };

private:
    PartSys* m_system;
    IntegrationMethod m_integrationMethod;
    glm::vec3 m_gravity;
    float m_drag;
    int numSprings;
    float groundKs;
    float groundKd;
    double prev_time = 0.0;
    double m_timeStep = 0.001;
    std::vector<glm::vec3> prev_positions; // 用于 Verlet 积分
};

#endif
