#pragma once
#ifndef MY_GRAVITY_SIMULATOR_H
#define MY_GRAVITY_SIMULATOR_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"

#include <string>

// a sample simulator

class ParticleSimulator : public BaseSimulator
{
public:

	ParticleSimulator(const std::string& name, BaseSystem* target);
	~ParticleSimulator();

	int step(double time);
	void reset(double time);
	int init(double time)
	{
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv) { return TCL_OK; }

protected:

	Vector m_pos0; // initial position
	Vector m_vel0; // initial velocity
	Vector m_pos;
	Vector m_vel;

	BaseSystem* m_object;

	double prev_time;
};


#endif