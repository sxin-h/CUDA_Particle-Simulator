/*
* FILE : integrator.h
* AUTHOR : Michael Davis
* DATE : 4/17/2017
*
* This is the integrator, this is where all of the integration functions are held to be used
*/

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include<iostream>
#include<glm.hpp>
using namespace glm;

enum iType {
	INTEGRATE_EULER,
	INTEGRATE_VERLET
};

//class Particle;
class Integrator {
public:
	
	Integrator();
	Integrator(iType type);
	void integrate_euler(vec3 * pos, vec3 * last_pos, double ts, vec3 * vel, vec3 * last_vel, vec3 acc);
	void integrate_verlet(vec3 *pos, vec3 *last_pos, double ts, float m, vec3 acc);
	//void integrate_verlet_velocity();
	void integrate_euler();
	iType getIType();
	void setIType(iType type);
	void printIType();
private:
	iType type;

};

#endif