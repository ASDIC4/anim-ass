#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(const std::string& name) :
	BaseSystem(name),
	m_sx(0.5f),
	m_sy(0.5f),
	m_sz(0.5f)
{
	setVector(m_pos, 0, 0, 0);

	double x = 5;
	double y = 5;
	double z = 0;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		int k = i * NUM_PROPERTIES;

		// Note the simple random rotation about the y axis
		double degrees = rand() % 360;
		double radians = degrees * PI / 180;
		double x_dir = x * cos(radians) + z * sin(radians); 
		double y_dir = y;
		double z_dir = -1.0 * x * sin(radians) + z * cos(radians);

		particles[k] = 0; // position
		particles[k + 1] = 0;
		particles[k + 2] = 0;
		particles[k + 3] = x_dir; // velocity
		particles[k + 4] = y_dir;
		particles[k + 5] = z_dir;
		particles[k + 6] = i / 10.0; // time til activated

		// Save the initial conditions so everything can be reset
		initial_particles[k] = particles[k + 0];
		initial_particles[k + 1] = particles[k + 1];
		initial_particles[k + 2] = particles[k + 2];
		initial_particles[k + 3] = particles[k + 3];
		initial_particles[k + 4] = particles[k + 4];
		initial_particles[k + 5] = particles[k + 5];
		initial_particles[k + 6] = particles[k + 6];
	}

}	// SampleParticle


void ParticleSystem::getState(double* p)
{
	for (int i = 0; i < NUM_PARTICLES * NUM_PROPERTIES; i++) {
		p[i] = particles[i];
	}
}	// SampleParticle::getState


void ParticleSystem::setState(double* p)
{
	for (int i = 0; i < NUM_PARTICLES * NUM_PROPERTIES; i++) {
		particles[i] = p[i];
	}
}	// SampleParticle::setState


void ParticleSystem::reset(double time)
{
	setState(initial_particles);
}	// SampleParticle::Reset


int ParticleSystem::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "read") == 0)
	{
		if (argc == 2)
		{
			m_model.ReadOBJ(argv[1]);
			glmFacetNormals(&m_model);
			glmVertexNormals(&m_model, 90);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: read <file_name>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "scale") == 0)
	{
		if (argc == 4)
		{
			m_sx = (float)atof(argv[1]);
			m_sy = (float)atof(argv[2]);
			m_sz = (float)atof(argv[3]);
		}
		else
		{
			animTcl::OutputMessage("Usage: scale <sx> <sy> <sz> ");
			return TCL_ERROR;

		}
	}
	else if (strcmp(argv[0], "pos") == 0)
	{
		if (argc == 4)
		{
			m_pos[0] = atof(argv[1]);
			m_pos[1] = atof(argv[2]);
			m_pos[2] = atof(argv[3]);
		}
		else
		{
			animTcl::OutputMessage("Usage: pos <x> <y> <z> ");
			return TCL_ERROR;

		}
	}
	else if (strcmp(argv[0], "flipNormals") == 0)
	{
		flipNormals();
		return TCL_OK;

	}
	else if (strcmp(argv[0], "reset") == 0)
	{
		reset(0.0);
	}

	glutPostRedisplay();
	return TCL_OK;

}	// SampleParticle::command


void ParticleSystem::display(GLenum mode)
{
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslated(m_pos[0], m_pos[1], m_pos[2]);
	glScalef(m_sx, m_sy, m_sz);

	if (m_model.numvertices > 0)
		glmDraw(&m_model, GLM_SMOOTH | GLM_MATERIAL);
	else
		glutSolidSphere(1.0, 20, 20);

	glPopMatrix();
	

	for (int i = 0; i < NUM_PARTICLES; i++) {
		int k = i * NUM_PROPERTIES;
		if (particles[k + 6] > 0) continue; // activation time not reached yet
		glPushMatrix();

		glTranslated(particles[k], particles[k + 1], particles[k + 2]);
		set_colour(1.0, 0, 0);
		glutSolidSphere(0.12, 20, 20);

		glPopMatrix();
	}

	
	glPopAttrib();
}	// SampleParticle::display
