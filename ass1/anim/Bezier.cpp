#include <math.h>
#include <vector>

#include "Bezier.h"

Bezier::Bezier(const std::string& name) : BaseSystem(name) {
    // Set the control points
    setVector(p0, -5, -5, 0);
    setVector(p1, -4, 5, 0);
    setVector(p2, 3, -5, 5);
    setVector(p3, 5, 3, 1);

    // Generate the samples of the curve uniformly in t
    // Do this the direct way on the polynomial 
    //setcontrolPointsDirect();

    // Do this the recursive way using de Casteljau
    setcontrolPointsdeCasteljau();
}


void Bezier::getState(double* p) {

}


void Bezier::setState(double* p) {

}


void Bezier::reset(double time) {

}


/// <summary>
/// This is actually the expanded form for 4 control points (3rd degree), ie a Cubic Bezier
/// not the de Casteljau approach itself (you can do it iteratively or recursively, see below)
/// This evaluates the curve given control points p0, p1, p2, p3 for dimension d (x, y, z) at parameter t in [0,1]
/// </summary>
/// <param name="d">Dimension (0, 1, 2) is (x, y, z) respectively</param>
/// <param name="t">Parameter in [0,1]</param>
/// <returns></returns>
double Bezier::evaluateCurveDirect(int d, double t) {
    return pow(1 - t, 3) * p0[d] + 3 * pow(1 - t, 2) * t * p1[d] + 3 * (1 - t) * pow(t, 2) * p2[d] + pow(t, 3) * p3[d];
}



/// <summary>
/// This iteratively samples points along the curve by plugging in uniformly sampled t params for x, y, and z
/// </summary>
void Bezier::setcontrolPointsDirect() {
    //double stepSize = 1.0 / double(numSamples - 1);
    double stepSize = 1.0 / double(numSamples);
    for (int i = 0; i < numSamples; i++) {
        double t = i * stepSize;
        controlPoints[i][0] = evaluateCurveDirect(0, t);
        controlPoints[i][1] = evaluateCurveDirect(1, t);
        controlPoints[i][2] = evaluateCurveDirect(2, t);
    }
}


/// <summary>
/// de Casteljau approach (you can do it iteratively or recursively on your own)
/// This evaluates the curve given control points p0, p1, p2, p3 for dimension d (x, y, z) at parameter t in [0,1].
/// Note that the way this is written sort of makes what is happening in the recursion clearer and also allows you to draw quadtratics.
/// Can you guess how you might go higher, for example quartics?
/// </summary>
/// <param name="d">Dimension (0, 1, 2) is (x, y, z) respectively</param>
/// <param name="t">Parameter in [0,1]</param>
/// <returns>Position in dimension d</returns>
double Bezier::evaluateCurvedeCasteljau(int d, double t) {
    return deCasteljauCubic(p0[d], p1[d], p2[d], p3[d], t);
}


/// <summary>
/// de Casteljau cubic level. You have four coefficients these come from your control points
/// p0, p1, p2, p3. Remember these have three dimensions each. We are going to evaluate each dimension seperately.
/// So when reading this chain of calls think in one dimension.
/// This is saying we need to do the interpolations on ABC (p0, p1, p2) and on BCD (p1, p2, p3), then interpolate the result.
/// This is the last interpolation in the slides! ABC and BCD will be one level down. Jump to that next, ie deCasteljauQuadratic
/// </summary>
/// <param name="A">A coefficient, ie control point 0</param>
/// <param name="B">B coefficient, ie control point 1</param>
/// <param name="C">C coefficient, ie control point 2</param>
/// <param name="D">D coefficient, ie control point 3</param>
/// <param name="t">Parameter in [0,1]</param>
/// <returns>Mixed value</returns>
double Bezier::deCasteljauCubic(double A, double B, double C, double D, double t) {
    double leftSide = deCasteljauQuadratic(A, B, C, t);
    double rightSide = deCasteljauQuadratic(B, C, D, t);

    return Lerp(leftSide, rightSide, t);
}


/// <summary>
/// de Casteljau quadratic level. You have three coefficients these come from your control points
/// Remember these have three dimensions each. We are going to evaluate each dimension seperately.
/// So when reading this chain of calls think in one dimension.
/// This is saying we need to do the linear interpolations on AB and on BC then interpolate the result.
/// Note that the next level down is just lerp! Jump to the lerp function next.
/// </summary>
/// <param name="A">A coefficient, ie control point 0</param>
/// <param name="B">B coefficient, ie control point 1</param>
/// <param name="C">C coefficient, ie control point 2</param>
/// <param name="t">Parameter in [0,1]</param>
/// <returns>Mixed value</returns>
double Bezier::deCasteljauQuadratic(double A, double B, double C, double t) {
    double leftSide = Lerp(A, B, t);
    double rightSide = Lerp(B, C, t);

    return Lerp(leftSide, rightSide, t);
}


/// <summary>
/// Just simple linear interpolation, will mix any value A with a value B by t
/// </summary>
/// <param name="t">Parameter in [0,1]</param>
/// <returns>Mixed value</returns>
double Bezier::Lerp(double A, double B, double t) {
    return ((1.0 - t) * A) + (t * B);
}


/// <summary>
/// This iteratively samples points along the curve by plugging in uniformly sampled t params for x, y, and z
/// </summary>
void Bezier::setcontrolPointsdeCasteljau() {
    double stepSize = 1.0 / double(numSamples - 1);
    for (int i = 0; i < numSamples; i++) {
        double t = i * stepSize;
        controlPoints[i][0] = evaluateCurvedeCasteljau(0, t);
        controlPoints[i][1] = evaluateCurvedeCasteljau(1, t);
        controlPoints[i][2] = evaluateCurvedeCasteljau(2, t);
    }
}


int Bezier::command(int argc, myCONST_SPEC char** argv) {
    glutPostRedisplay();
    return TCL_OK;
}


/// <summary>
/// Displays a single control point given the point and radius
/// </summary>
/// <param name="p">point</param>
/// <param name="r">radius</param>
void Bezier::displayControlPoint(Vector p, float r) {
    glPointSize(r);
    glBegin(GL_POINTS);
    glVertex3dv(p);
    glEnd();
}


/// <summary>
/// Display all of the sample points given their radius (points stored in the class, see Bezier.h)
/// </summary>
/// <param name="r">The radius of the point rendering</param>
void Bezier::displaycontrolPoints(float r) {
    glPointSize(r);

    glBegin(GL_POINTS);
    for (int i = 0; i < numSamples; i++)
        glVertex3dv(controlPoints[i]);

    glEnd();
}


/// <summary>
/// Render the curve as a polyline approximation (ie draw lines between the sample points)
/// </summary>
void Bezier::displaySampledCurve(float r) {
    glLineWidth(r);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < numSamples; i++)
        glVertex3f(controlPoints[i][0], controlPoints[i][1], controlPoints[i][2]);
    glEnd();
}


void Bezier::display(GLenum mode) {
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_COLOR_MATERIAL);

    // Draw the control points
    glColor3f(1, 0, 0);
    displayControlPoint(p0, 10.0);
    displayControlPoint(p1, 10.0);
    displayControlPoint(p2, 10.0);
    displayControlPoint(p3, 10.0);

    // Draw the intermediate samples along the curve
    glColor3f(0, 0, 0);
    displaycontrolPoints(3.0);

    // Draw the lines in between the samples
    glColor3f(0.3, 0.7, 0.1);
    displaySampledCurve(1.5);

    glPopAttrib();
}
