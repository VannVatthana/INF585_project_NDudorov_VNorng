#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "simulation/simulation.hpp"
#include "voronoi/voronoi.hpp"


struct gui_parameters {
	bool explode = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_basic timer;
	float explosion_timer = 0;
	float explosion_period = 0.05;
	float flame_period = 0.0025;

	// *********** //
	// hard-coded building and wall parameters
	// *********** //
	float Dx=1.650;float Dy=1.05;float Dz=0.01; //[-Dx,Dx] x [-Dy, Dy] x [-Dz, Dz] wall
	float d_x=2.211; float d_y=1.670; float d_z=2.830; //distance between walls
	int walls_per_horizontal = 7; int walls_per_vertical = 5; int side_walls_per_horizontal = 3;
	const cgp::vec3 front_left_low_corner=cgp::vec3(-16.495,3.196,8.969);  
	const cgp::vec3 back_left_low_corner=cgp::vec3(-16.495,3.196,-8.969);
	const cgp::vec3 lside_left_low_corner=cgp::vec3(-19.175,3.196,-6.082); 
	const cgp::vec3 rside_left_low_corner=cgp::vec3(19.175,3.196,-6.082);  

	//cgp::skybox_drawable skybox; 
	cgp::mesh_drawable ground; // for the scene appearence
	cgp::mesh_drawable building; // building with some walls
	mesh_drawable wall; // to visualize wall to explode
	std::vector<broken_wall> broken_walls; //exploded walls
	std::vector<dust_particle> dust_particles; //dust particles to keep position of dust cloud
	cgp::mesh_drawable dust_billboard; // billboard to imtiate dust cloud
	std::vector<flame_particle> flame_particles; //flame particles to keep flame position and animation
	std::vector<cgp::mesh_drawable> flame_billboards; //billboards to imitate flames of explosion 


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop
	void draw_ground();
	void draw_broken_wall(broken_wall &b_wall, const vec3 &translation);

	void initialize_ground();
	void initialize_building();
	void initialize_wall();
	void initialize_broken_walls();
	void initialize_dust();
	void initialize_flame();
	void initialize_explosion();

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





