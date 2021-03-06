#include "ViewController.h"


ViewController::ViewController(Simulation *sim)
{
	_sim = sim;

	_sdl_window = 0;
	_sdl_glcontext = 0;
	_quit = false;

	_x_angle_init = 0.0f;
	_y_angle_init = 0.0f;
	_x_tran_init = 0.0f;
	_y_tran_init = 0.0f;
	_x_angle = 0;
	_y_angle = 0;
	_firstclick_r = true;
	_firstclick_l = true;

	_rot_base_x = 0.0f;
	_rot_base_y = 0.0f;
	_tran_base_x = 0.0f;
	_tran_base_y = 0.0f;

}

bool ViewController::init()
{
	// Initialive SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "Failed to initialize SDL.\n";
		return false;
	}

	// Create the SDL Window
	if ((_sdl_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL)) == NULL)
	{
		cout << "Failed to create window.\n";
		return false;
	}

	// Create OpenGL Context in the SDL Window
	if ((_sdl_glcontext = SDL_GL_CreateContext(_sdl_window)) == NULL)
	{
		cout << "Failed to create SDl GL Context.\n";
		return false;
	}

	return true;
}

void ViewController::setAttributes()
{
	// Set OpenGL SDl Attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
}

void ViewController::display()
{
	// Draw the model here
	m.draw();
	SDL_GL_SwapWindow(_sdl_window);
}

void ViewController::handleEvents(SDL_Event e)
{
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			_quit = true;
			break;

		// Simulation controls.
		case SDL_KEYDOWN:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				_quit = true;
				break;
			case SDLK_s:
				_sim->start();
				break;
			case SDLK_r:
				// Cover with red and update
				_sim->reset();
				m.initAssets();
				break;
			case SDLK_o:
				_sim->OTH->toggleRender();
				break;
			default:
				break;
			}
			break;
		}

		// Model Zoom
		case SDL_MOUSEWHEEL:
		{
			if (e.wheel.y > 0)
			{
				_zoom -= 0.1f;
			}
			else if (e.wheel.y < 0)
			{
				_zoom += 0.1f;
			}
			if (_zoom <= 0.0) _zoom = 0.0001;
			m.update(_x_angle, _y_angle, _zoom, _trans.x, _trans.y);
			break;
		}

		// Model Rotation and transform controls
		case SDL_MOUSEBUTTONDOWN:
		{
			if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_LEFT))  //Attach rotation to the left mouse button
			{
				// save position where button down event occurred. This
				// is the "zero" position for subsequent mouseMotion callbacks.
				_rot_base_x = e.button.x;
				_rot_base_y = e.button.y;
				_rotating = true;
			}
			if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_RIGHT))  //Attach rotation to the left mouse button
			{
				// save position where button down event occurred. This
				// is the "zero" position for subsequent mouseMotion callbacks.
				_tran_base_x = e.button.x;
				_tran_base_y = e.button.y;
				_transforming = true;
			}
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (_rotating)  //are we finishing a rotation?
			{
				//Remember where the motion ended, so we can pick up from here next time.
				_last_rot_offset_x += (e.button.x - _rot_base_x);
				_last_rot_offset_y += (e.button.y - _rot_base_y);
				_rotating = false;
			}
			if (_transforming)  //are we finishing a rotation?
			{
				//Remember where the motion ended, so we can pick up from here next time.
				_last_tran_offset_x += (e.button.x - _tran_base_x);
				_last_tran_offset_y += (e.button.y - _tran_base_y);
				_transforming = false;
			}
			break;
		}
		case SDL_MOUSEMOTION:
		{
			//Is the left mouse button also down?
			if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_LEFT))
			{

				float x, y;

				//Calculating the conversion => window size to angle in degrees
				float scaleX = 120.0 / WINDOW_WIDTH;
				float scaleY = 120.0 / WINDOW_HEIGHT;

				x = (e.button.x - _rot_base_x) + _last_rot_offset_x;
				y = (e.button.y - _rot_base_y) + _last_rot_offset_y;

				// map "x" to a rotation about the y-axis.
				x *= scaleX;
				_y_angle = x;

				// map "y" to a rotation about the x-axis.
				y *= scaleY;
				_x_angle = y;

				if (_firstclick_l)
				{
					_x_angle_init = _x_angle;
					_y_angle_init = _y_angle;
					_firstclick_l = false;
				}

				_y_angle -= _y_angle_init;
				_x_angle -= _x_angle_init;

				m.update(_x_angle, _y_angle, _zoom, _trans.x, _trans.y); //send the new angles to the Model object
			}

			if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(SDL_BUTTON_RIGHT))
			{

				float x, y;

				//Calculating the conversion => window size to angle in degrees
				float scaleX = 50.0f / WINDOW_WIDTH;
				float scaleY = 50.0f / WINDOW_HEIGHT;

				x = (e.button.x - _tran_base_x) + _last_tran_offset_x;
				y = (e.button.y - _tran_base_y) + _last_tran_offset_y;

				// map "x" to a rotation about the y-axis.
				x *= scaleX;
				_trans.x = x;

				// map "y" to a rotation about the x-axis.
				y *= scaleY;
				_trans.y = -y;

				if (_firstclick_r)
				{
					_x_tran_init = _trans.x;
					_y_tran_init = _trans.y;
					_firstclick_r = false;
				}

				_trans.x -= _x_tran_init;
				_trans.y -= _y_tran_init;

				m.update(_x_angle, _y_angle, _zoom, _trans.x, _trans.y); //send the new angles to the Model object
			}
			break;
		}

		// End of event switch.
		}
	}
}

void ViewController::cleanup()
{
	// Cleanup SDL
	SDL_GL_DeleteContext(_sdl_glcontext);
	SDL_DestroyWindow(_sdl_window);
	SDL_Quit();
}

void ViewController::run()
{
	if (!init())
	{
		cout << "Failed to initialize View Controller.\n";
		return;
	}

	if (!m.init(_sim))
	{
		cout << "Failed to initialize Model Object.\n";
		return;
	}

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(_sdl_window);

	//incrementalUpdateMethod();
	finiteUdateMethod();

	cleanup();

}

//TODO : Fix issue when NUM_FRMAES_PER SECOND is less than MAX_FPS
void ViewController::finiteUdateMethod()
{
	int i;

	// Initialize the accumulative time to caculate FPS
	acc_time = {0,0};

	struct timespec RENDER_STEP = {0, BILLION / MAX_FPS};

	// Initialize fram and update counters
	int frame_count = 0;
	int update_count = 0;

	struct timespec lost, temp;
	// Main Application loop, this loop handles both the calls for physics
	// simulation updates and graphics updates. This loop handles timing,
	// ensuring that what is displayed to the user is at fastest real time
	// OpenGL/SDL cap rendering to 60 fps. Therefore multiple simulation
	// updates will be performed per render.
	do
	{
		// Get the start time of the frame before rendering.
		clock_gettime(CLOCK_MONOTONIC, &start_frame);

		// Display the simulation with OpenGL and handle SDL events.
		display();
		handleEvents(_event);

		// Increment the frame count.
		frame_count++;

		// 0 the update time acculilator
		acc_up = {0,0};
		lost = {0,0};

		// Get the start time for this update.
		clock_gettime(CLOCK_MONOTONIC, &start_up);

		if(update_count < UPDATES_PER_SECOND){
			for (i = 0; i < UPDATES_PER_SECOND / MAX_FPS; i++)
			{
				// run simulation update and increment counter.
				_sim->update();
				update_count++;
			}
			// TODO Fix timing inaccuracy, this lags the animation at the end of every second. Need to distribute updates more evenly.
			// if (frame_count == MAX_FPS)
			// {
			// 	int buff_up = UPDATES_PER_SECOND - ((int)(UPDATES_PER_SECOND / MAX_FPS) * MAX_FPS);
			// 	//cout << "buff_up : " << buff_up << endl;
			// 	for (i = 0; i < buff_up; i++)
			// 	{
			// 		_sim->update();
			// 		update_count++;
			// 	}
			// }
		}
		// Get the final time of the update.
		clock_gettime(CLOCK_MONOTONIC, &end_up);

		// get delta time for update.
		delta_up = timing::diff_time(start_frame, end_up);

		if (delta_up.tv_sec <= RENDER_STEP.tv_sec && delta_up.tv_nsec < RENDER_STEP.tv_nsec){
			wait = timing::diff_time(delta_up, _sim->TIME_STEP);
			if (nanosleep(&wait, &temp)!=0)
			{
				//cout << "Failed to sleep properly.\n";
				nanosleep(&temp, nullptr);
			}
		}


		clock_gettime(CLOCK_MONOTONIC, &end_frame);
		delta_frame_time = timing::diff_time(start_frame, end_frame);
		// Accumilate the time for calculating FPS.
		acc_time = timing::add_time(acc_time, delta_frame_time);

		// After one full second, report the FPS and UPF( Average calculated)
		if (acc_time.tv_sec >= 1)
		{
			string info_title = string(WINDOW_TITLE) +
						"\tFPS : " + to_string(frame_count) +
						"\tUPS : " + to_string(update_count) +
						"\tAVG UPF : " + to_string((float)update_count / (float)frame_count);
			SDL_SetWindowTitle(_sdl_window, info_title.c_str());
			cout << info_title << endl;

			frame_count = 0;
			update_count = 0;
			acc_time = {0,0};
		}

	} while (!_quit);
}

void ViewController::incrementalUpdateMethod()
{
	// Initialize the accumulative time to caculate FPS
	acc_time = {0,0};

	// Initialize fram and update counters
	int frame_count = 0;
	int update_count = 0;

	struct timespec lost, temp;
	// Main Application loop, this loop handles both the calls for physics
	// simulation updates and graphics updates. This loop handles timing,
	// ensuring that what is displayed to the user is at fastest real time
	// OpenGL/SDL cap rendering to 60 fps. Therefore multiple simulation
	// updates will be performed per render.
	do
	{
		// Get the start time of the frame before rendering.
		clock_gettime(CLOCK_MONOTONIC, &start_frame);

		// Display the simulation with OpenGL and handle SDL events.
		display();
		handleEvents(_event);

		// Increment the frame count.
		frame_count++;

		// 0 the update time acculilator
		acc_up = {0,0};
		lost = {0,0};
		do
		{
			// Get the start time for this update.
			clock_gettime(CLOCK_MONOTONIC, &start_up);

			// run simulation update and increment counter.
			_sim->update();
			update_count++;

			// Get the final time of the update.
			clock_gettime(CLOCK_MONOTONIC, &end_up);

			// get delta time for update.
			delta_up = timing::diff_time(start_up, end_up);

			// If the final time of the update is less that the TIME STEP
			// of the simulation then wait until that is met. This helps
			// run the simulation in real time if it is simple enough.

			if (delta_up.tv_sec <= _sim->TIME_STEP.tv_sec && delta_up.tv_nsec < _sim->TIME_STEP.tv_nsec && _sim->sim_state == RUNNING){
				wait = timing::diff_time(delta_up, _sim->TIME_STEP);
				if (nanosleep(&wait, &temp)!=0)
				{
					cout << "Failed to sleep properly.\n";
					lost = timing::add_time(lost, temp);
				}
			}


			// Check to see if the time since the beginning of this loop is
			// larger or equal to 1/60 seconds, if so stop running updates.
			clock_gettime(CLOCK_MONOTONIC, &end_frame);
			delta_frame_time = timing::diff_time(start_frame, end_frame);
			if (delta_frame_time.tv_sec > 0 || delta_frame_time.tv_nsec > BILLION / 60) break;

		}while(true);
		// Accumilate the time for calculating FPS.
		acc_time = timing::add_time(acc_time, delta_frame_time);

		// After one full second, report the FPS and UPF( Average calculated)
		if (acc_time.tv_sec >= 1)
		{
			cout << "FPS : " << frame_count <<
				  "\tUPS : " << update_count <<
				  "\tAVG UPF : " << update_count / frame_count <<
				  endl;

			frame_count = 0;
			update_count = 0;
			acc_time = {0,0};
		}

	} while (!_quit);
}
