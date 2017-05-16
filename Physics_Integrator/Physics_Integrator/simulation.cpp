/*
* FILE : simulation.cpp
* AUTHOR : Michael Davis
* DATE : 4/17/2017
*
* This is the primary simulation class, it handles simulation time, updates, simulation state and
* rendering of the simulation environment and simulation objects
*/

#include <iostream>
using namespace std;
#include "simulation.h"

// ********************************************************************************
// *                               -- PUBLIC --                                   *
// ********************************************************************************

// -------------------- Simulation Constructors and Destructor --------------------

Simulation::Simulation()
{
	if (!init()) 
	{
		cout << "Failed to initialize simulation.\n";
		exit(-4);
	}
}

Simulation::~Simulation()
{
	particles.clear();
}


// -------------------- Simulation Control Functions --------------------

bool Simulation::start()
{
	if (sim_state == INITIALIZED) 
	{
		cout << "Starting Simulation of "<< PARTICLE_COUNT << " falling particles." << endl;
		integrator.printIType();
		sim_state = RUNNING;

		sim_time_accu = 0;

		// Get ticks per second.
		QueryPerformanceFrequency(&frequency);
		// Start timer.
		QueryPerformanceCounter(&t1);
		
		return true;
	}
	return false;
}

bool Simulation::pause()
{
	if (sim_state == PAUSED) {
		sim_state = RUNNING;
		cout << "Simulation Running.\n";
		QueryPerformanceCounter(&t1);
		return true;
	}
	else if (sim_state == RUNNING) {
		sim_state = PAUSED;
		cout << "Simulation Paused at : " << sim_time_accu << " seconds\n";
		return true;
	}
	return false;
}

bool Simulation::end()
{
	if (sim_state >= RUNNING && sim_state != FINISHED) {
		cout << "Simulation has ended.\n";
		cout << "Simulation ran for : " << sim_time_accu << "\n\n";
	}
	sim_state = FINISHED;
	return false;
}

bool Simulation::reset()
{
	end();
	cout << "Resetting Simulation.\n";
	printControls();
	clean();
	init();
	return false;
}


// -------------------- Simulation Step Functions --------------------

void Simulation::update(float rdt)
{
	if (sim_state == RUNNING)
	{
		// Using high res. counter.
		QueryPerformanceCounter(&t2);

		// Compute the elapsed time in seconds.
		frame_time = ((t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart) / 1000.0;
		t1 = t2;
		sim_time_accu += frame_time;

		// Update every particle and check to see if all particles have hit the floor.
		//bool checkSimOver = true;
		for (Particle* p : particles)
		{
			p->update(frame_time, glm::vec3(0.0f, 0.0f, 0.0f), &integrator);
			
			//// ----- starting collision -----
			vec3 curr_pos = p->pos;
			vector<Plane*> col_planes = getCollisions(p, planes);
			if (col_planes.size() > 0)
			{
				//cout << "---------------------------" << "\n";
				//cout << "Num of cols : " << col_planes.size() << "\n";
			}
			// ----- end collision -----
			//if (p->pos.y > 0) checkSimOver = false;
		}

		//if (checkSimOver) {
		//	end();
		//}
	}
}

void Simulation::render()
{
	if (sim_state >= INITIALIZED)
	{
		// Render all simulation objects here. 
		for (Plane* pl : planes) 
		{
			pl->draw();
		}
		
		for (Particle* p : particles)
		{
			p->draw();
		}
		//drawOrientationKey();
	}
}


// -------------------- Getter Functions --------------------

double Simulation::getSimFrameTime() { return frame_time; }

int Simulation::getSimCurrState() {	return sim_state; }


// -------------------- GPrint Functions --------------------

void Simulation::printControls()
{
	cout << "---------- Simulation Controls ----------\n";
	cout << "\t s : Start the simulation.\n";
	cout << "\t p : Pause and Un-Pause the simulation.\n";
	cout << "\t r : Reset the simulation.\n\n";
}

// ********************************************************************************



// ********************************************************************************
// *                               -- Private --                                  *
// ********************************************************************************

bool Simulation::init()
{
	// This is where all simulation object will be initialized to set up the simulation.
	integrator.setIType(INTEGRATE_VERLET);
	//integrator.setIType(INTEGRATE_EULER);

	//planes.push_back(new Plane(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), 20.0f));
	//planes.push_back(new Plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 2.0f, 3.0f), 5.0f));

	planes.push_back(new Plane(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), 10.0f));
	planes.push_back(new Plane(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 10.0f));
	planes.push_back(new Plane(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 10.0f));
	planes.push_back(new Plane(glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f));
	//planes.push_back(new Plane(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), 10.0f));
	planes.push_back(new Plane(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f), 10.0f));

	///*
	glm::vec3 ranPos;
	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		ranPos.x = -planes[0]->width + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * planes[0]->width)));
		ranPos.z = -planes[0]->width + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * planes[0]->width)));
		ranPos.y = 5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (8)));
		particles.push_back(new Particle(ranPos));
	}
	//*/

	//particles.push_back(new Particle(glm::vec3(0.0f, 0.0f, 0.0f)));

	sim_state = INITIALIZED;
	return true;
}

void Simulation::drawOrientationKey()
{
	//draw orientation key
	glBegin(GL_LINES);

	//Red is Positive X
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);

	//Green is positive Y
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);

	//Blue is posiotive Z
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);

	glEnd();
}


void Simulation::clean()
{
	// De-allocate all of the particles from the vector.
	while (!particles.empty()) {
		delete particles.back();
		particles.pop_back();
	}
	while (!planes.empty()) {
		delete planes.back();
		planes.pop_back();
	}
}

float Simulation::distFromPlane(vec3 pos, Plane * plane)
{
	return dot(plane->normal, (pos - plane->centre));
}

vector<Plane*> Simulation::getCollisions(Particle * p, vector<Plane*> planes)
{
	vector<Plane*> col_planes;
	for (Plane* pl : planes) {
		float t1 = distFromPlane(p->pos, pl);
		//
		//
		float t3 = distFromPlane(p->next_pos, pl);
		if (t1 > 0 && t3 <= 0)
		{
			col_planes.push_back(pl);
			p->pos = reflect(p->pos, pl);
			p->next_pos = reflect(p->next_pos, pl);
		}
	}
	return col_planes;
}

vec3 Simulation::reflect(vec3 pos, Plane * plane)
{
	return pos + 2 * (-distFromPlane(pos, plane) / dot(plane->normal, plane->normal)) * plane->normal;
}

// ********************************************************************************