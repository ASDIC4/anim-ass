#ifndef MY_BEZIER_H
#define MY_BEZIER_H

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

class Bezier : public BaseSystem
{

public:
	Bezier(const std::string& name);
	virtual void getState(double* p);
	virtual void setState(double* p);
	void reset(double time);

	// The direct method evaluate the full function
	double evaluateCurveDirect(int d, double t);
	void setcontrolPointsDirect();

	// The de Casteljau method of repeated linear interpolations
	double evaluateCurvedeCasteljau(int d, double t);
	double deCasteljauCubic(double A, double B, double C, double D, double t);
	double deCasteljauQuadratic(double A, double B, double C, double t);
	double Lerp(double A, double B, double t);
	void setcontrolPointsdeCasteljau();

	void displayControlPoint(Vector p, float r);
	void displaycontrolPoints(float r);
	void displaySampledCurve(float r);
	void display(GLenum mode = GL_RENDER);

	int command(int argc, myCONST_SPEC char** argv);

protected:
	int numSamples = 40;
	Vector p0, p1, p2, p3;
	Vector controlPoints[40];
};
#endif
