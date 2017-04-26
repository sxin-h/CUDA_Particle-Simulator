/*
* FILE : particle.cpp
* AUTHOR : Michael Davis
* DATE : 4/17/2017
*
* TParticle class of the simulation, mostly just holds basic values about the particles information
*/


#include <glew.h>
#include <iostream>
using namespace std;
#include "particle.h"

// ********************************************************************************
// *                               -- PUBLIC --                                   *
// ********************************************************************************

// -------------------- Particle Constructors and Destructor --------------------

Particle::Particle()
{
	if (!init(glm::vec3(0,0,0))) 
	{
		cout << "Failed to initialize Particle" << endl;
		system("PAUSE");
		exit(-2);
	}
}

Particle::Particle(glm::vec3 p)
{
	if (!init(p)) {
		cout << "Failed to initialize Particle" << endl;
		system("PAUSE");
		exit(-2);
	}
}

Particle::~Particle() { }


// -------------------- Simulation Step Functions --------------------

void Particle::update(double time_step, glm::vec3 global_forces, Integrator *i)
{
	//We will be updateing the particle here, this is where the verlet integration will be used to update the position 
	//We could also add a variable to integrate via Euler later on in the prtoject
	
	// Compute forces on particle
	//force = global_forces;

	// Compute Verlet integration

	acc = glm::vec3(0.0f, -9.81f, 0.0f);
	//acc += global_forces / mass;

	i->integrate_verlet(&pos, &last_pos, time_step, mass, acc);

	// for this simulation make the particle rest on the ground.
	if (pos.y < 0)
		pos.y = 0;
}

void Particle::draw()
{
	//cout << "Position :	X =" << pos.x << "\t Y = " << pos.y << "\t Z = " << pos.z << endl;
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(pos.x, pos.y, pos.z);
	glEnd();
}


// ********************************************************************************
// *                               -- Private --                                  *
// ********************************************************************************

bool Particle::init(glm::vec3 p)
{
	//initialize last pos, pos, mass, and force here to initialize the particle
	pos = p;
	last_pos = pos;
	mass = 1000.0f;

	return true;
}