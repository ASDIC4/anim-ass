#include "ParticleSimulator.h"
#include "ParticleSystem.h"

ParticleSimulator::ParticleSimulator(const std::string& name, BaseSystem* target) :
	BaseSimulator(name),
	m_object(target),
	prev_time(0.0)
{
}	// SampleGravitySimulator


ParticleSimulator::~ParticleSimulator()
{
}	// SampleGravitySimulator::~SampleGravitySimulator


int ParticleSimulator::step(double time)
{
	double dt = time - prev_time;
	prev_time = time;
	double g = -9.8;
	double particles[NUM_PARTICLES * NUM_PROPERTIES];
	m_object->getState(particles);

	// Simple update loop that assumes particles do not influence each other
	for (int i = 0; i < NUM_PARTICLES; i++) {
		int k = i * NUM_PROPERTIES;

		// Timer until particle is active (note the continue statement)
		if (particles[k + 6] > 0) {
			particles[k + 6] -= dt;
			continue;
		}

		// Update Velocity, note the Y axis of velocity is in the 4th state position
		// [ 0   ,  1   ,  2   ,  3   ,  4   ,  5   , 6]
		// [pos x, pos x, pos x, vel x, vel y, vel z, t]
		particles[k + 4] = particles[k + 4] + g * dt;

		// Update Position, note the position and velocity components in the state
		// [ 0   ,  1   ,  2   ,  3   ,  4   ,  5   , 6]
		// [pos x, pos x, pos x, vel x, vel y, vel z, t]
		particles[k] = particles[k] + particles[k + 3] * dt;
		particles[k + 1] = particles[k + 1] + particles[k + 4] * dt;
		particles[k + 2] = particles[k + 2] + particles[k + 5] * dt;
	}

	m_object->setState(particles);

	return 0;

}	// SampleGravitySimulator::step


void ParticleSimulator::reset(double time) {
	prev_time = 0;
}