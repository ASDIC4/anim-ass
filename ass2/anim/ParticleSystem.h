#pragma once
#ifndef MY_PARTICLE_H
#define MY_PARTICLE_H

#define NUM_PARTICLES 50
// Properties of state are
// position (3) + velocity (3) + time until active (1)
#define NUM_PROPERTIES 7


#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

// a sample system
class ParticleSystem : public BaseSystem
{

public:
	ParticleSystem(const std::string& name);
	virtual void getState(double* p);
	virtual void setState(double* p);
	void reset(double time);

	void display(GLenum mode = GL_RENDER);

	void readModel(char* fname) { m_model.ReadOBJ(fname); }
	void flipNormals(void) { glmReverseWinding(&m_model); }
	int command(int argc, myCONST_SPEC char** argv);

protected:

	float m_sx;
	float m_sy;
	float m_sz;

	Vector m_pos;

	// The following store state at init adn throughout sim
	// State is: position (3) + velocity (3) + time until active (1)
	double initial_particles[NUM_PARTICLES * NUM_PROPERTIES];
	double particles[NUM_PARTICLES * NUM_PROPERTIES]; 

	GLMmodel m_model;

};
#endif
