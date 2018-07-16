#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "GL/glew.h"
#include <SDL2/SDL.h>

#include "Model.h"
#include "../Sim/Simulation.h"
#include <thread>

#include <iostream>
using namespace std;

class ViewController
{
private:
	const char WINDOW_TITLE[32] = "Verlet_CUDA_Particle_Simulator";
	const int WINDOW_WIDTH = 1600;
	const int WINDOW_HEIGHT = 1200;

	SDL_Window *_sdl_window;
	SDL_GLContext _sdl_glcontext;
	SDL_Event _event;
	bool _quit;

	bool _rotating;
	float _base_x;
	float _base_y;
	float _last_offset_x;
	float _last_offset_y;
	float _x_angle;
	float _y_angle;
	float _x_angle_init;
	float _y_angle_init;
	bool _firstclick;

	float _zoom = 1.0f;

	Simulation *_sim;
	Model m;
	vec3 p = normalize(vec3(243, 27, 227));

	bool init();
	void setAttributes();
	void display();
	void handleEvents(SDL_Event e);
	void cleanup();

	void cc(vec3 c)
	{
		glClearColor(c.x, c.y, c.z, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(_sdl_window);
	}

public:

	ViewController(Simulation *sim);
	void run();
};

#endif
