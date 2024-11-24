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
    SampleParticle* m_particle; // �����Ƶ�����
    HermiteSpline* m_spline;    // ����·��
    double m_startTime;         // ������ʼʱ��
    double m_currentTime;       // ��ǰʱ��
    double m_totalTime;         // �ܵ��˶�ʱ��
    double m_speed;             // �˶��ٶ�
};

#endif // HERMITE_SIMULATOR_H
