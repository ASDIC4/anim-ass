#ifndef HERMITE_SIMULATOR_H
#define HERMITE_SIMULATOR_H

#include "BaseSimulator.h"
#include "SampleParticle.h"
#include "HermiteSpline.h"

class HermiteSimulator : public BaseSimulator {
public:
    HermiteSimulator(const std::string& name, SampleParticle* particle, HermiteSpline* spline);

    int step(double time) override;
    int init(double time) override;

private:
    SampleParticle* m_particle; // 被控制的物体
    HermiteSpline* m_spline;    // 样条路径
    double m_startTime;         // 动画开始时间
    double m_currentTime;       // 当前时间
    double m_totalTime;         // 总的运动时间
    double m_speed;             // 运动速度
};

#endif // HERMITE_SIMULATOR_H
