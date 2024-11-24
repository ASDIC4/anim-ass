#include "HermiteSimulator.h"
#include <iostream>

HermiteSimulator::HermiteSimulator(const std::string& name, SampleParticle* particle, HermiteSpline* spline)
    : BaseSimulator(name), m_particle(particle), m_spline(spline), m_startTime(0.0), m_currentTime(0.0), m_totalTime(0.0), m_speed(0.05) // ��ʼ�� m_startTime �� m_totalTime
{}

int HermiteSimulator::init(double time) {
    m_startTime = time;
    m_currentTime = time;
    m_particle->reset(time);
    return 0;
}

int HermiteSimulator::step(double time) {
    double deltaTime = time - m_currentTime;
    m_currentTime = time;

    // ʹ���ۻ���ʱ�������������ϵ�λ��
    double t = (m_currentTime - m_startTime) * m_speed;
    if (t > 1.0) {
        t = 1.0; // �����յ�
    }

    // ���� SampleParticle �� updatePosition ����
    m_particle->updatePosition(t);

    // ÿ���ӡ�ٶ���Ϣ
    if (static_cast<int>(time) % 1 == 0) {
        std::cout << "Current speed: " << m_speed << std::endl;
    }

    return 0;
}
