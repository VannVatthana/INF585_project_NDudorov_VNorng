#include "scene.hpp"


using namespace cgp;


void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	//skybox.initialize_data_on_gpu(project::path+"assets/skybox/");
	initialize_ground();
	initialize_building();
	initialize_wall();
	initialize_broken_walls();
	initialize_dust();
	initialize_flame();
	initialize_explosion();
}

void scene_structure::initialize_ground()
{
	float L = 10;
	mesh ground_mesh = mesh_primitive_quadrangle({-L,0,-L}, {-L,0,L},{L,0,L },{L,0,-L}); 
	ground.initialize_data_on_gpu(ground_mesh);
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path+"assets/ground.jpg",GL_REPEAT,GL_REPEAT);
	ground.material.phong.specular=0;
}

void scene_structure::initialize_building()
{
	vec3 color={0.3,0.3,0.3};
	float ambient = 0.9f;
	float diffuse = 1.0f;
	float specular = 0.0f;
	float specular_exponent = 8.0f;

	mesh building_mesh = mesh_load_file_obj(project::path+"data/polybuilding_nowin.obj");
	building.initialize_data_on_gpu(building_mesh);
	building.material.color = color;
	building.material.phong = {ambient,diffuse,specular,specular_exponent};
}

void scene_structure::initialize_wall()
{
	vec3 color={0.8,0.8,0.9};
	float ambient = 0.9f;
	float diffuse = 0.1f;
	float specular = 0.9f;
	float specular_exponent = 64.0f;
	float transparency = 0.5f;

	//wall dimensions [-Dx,Dx] x [-Dy,Dy] x [-Dz,Dz]
	wall.initialize_data_on_gpu(mesh_primitive_cubic_grid({-Dx,-Dy,-Dz},{Dx,-Dy,-Dz},{Dx,Dy,-Dz},{-Dx,Dy,-Dz},
														  {-Dx,-Dy,Dz},{Dx,-Dy,Dz},{Dx,Dy,Dz},{-Dx,Dy,Dz})); 
    wall.material.color = color;
	wall.material.phong = {ambient,diffuse,specular,specular_exponent};
	wall.material.alpha = transparency;
}

void scene_structure::initialize_broken_walls()
{
	broken_walls.clear();

	vec3 color={0.8,0.8,0.9};
	float ambient = 0.9f;
	float diffuse = 0.1f;
	float specular = 0.9f;
	float specular_exponent = 64.0f;
	float transparency = 0.8f;
	
	for(int i = 0; i < walls_per_vertical; ++i)
	{
		for(int j = 0; j < walls_per_horizontal; ++j)
		{
			broken_wall b_wall;
			b_wall.initialize();
			for(mesh_drawable &mesh : b_wall.meshes)
			{
				mesh.material.color = color;
				mesh.material.phong = {ambient,diffuse,specular,specular_exponent};
				mesh.material.alpha = transparency;
			}
			broken_walls.push_back(b_wall);
		}
	}
}

void scene_structure::initialize_dust()
{
	dust_particles.clear();

	float const L = 3.5*1.5; //size of the billboard
	dust_billboard.initialize_data_on_gpu(mesh_primitive_quadrangle({ -L,-L,0 }, { L,-L,0 }, { L,L,0 }, { -L,L,0 }));
	dust_billboard.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/dust1.png");

	float diametr = 0.5*0.05; //particle's size
	for(int i = 0; i < walls_per_vertical; ++i)
	{
		for(int j = 0; j < walls_per_horizontal; ++j)
		{
			vec3 position = front_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
			dust_particle particle(diametr,position);
			dust_particles.push_back(particle);
		}
	}
}

void scene_structure::initialize_flame()
{
	flame_particles.clear();
	flame_billboards.clear();

	image_structure const im_explosion = image_load_file(project::path + "assets/explosion_8x6.png");
	int Frames_Per_Vertical = 6;
	int Frames_Per_Horizontal = 8;

	std::vector<image_structure> frame_explosion; 
	int w_new = (int)im_explosion.width / Frames_Per_Horizontal;
	int h_new = (int)im_explosion.height / Frames_Per_Vertical;
	for (int y = Frames_Per_Vertical-1; y >= 0; y--)
		for (int x = 0; x < Frames_Per_Horizontal; x++)
			frame_explosion.push_back(im_explosion.subimage(x * w_new, y * h_new, (x + 1) * w_new - 1, (y + 1) * h_new - 1));
	
	// Load series of explosion billboards
	float const L = 2.0f; //size of the quad
	for (int frame = 0; frame <frame_explosion.size(); ++frame)
	{
		cgp::mesh_drawable billboard2add;
		billboard2add.initialize_data_on_gpu(mesh_primitive_quadrangle({ -L,-L,0 }, { L,-L,0 }, { L,L,0 }, { -L,L,0 }));
		billboard2add.texture.initialize_texture_2d_on_gpu(frame_explosion[frame]);
		flame_billboards.push_back(billboard2add);
	}

	for(int i = 0; i < walls_per_vertical; ++i)
	{
		for(int j = 0; j < walls_per_horizontal; ++j)
		{
			vec3 position = front_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
			flame_particle particle;
			particle.position = position;
			flame_particles.push_back(particle);
		}
	}
}


void scene_structure::initialize_explosion()
{
	vec3 source = vec3(0.,0.,-1.25); //position of the center of explosion 
	float A = 1000.; //explosion force amplitude
	float W = 20; //angular velocity amplitude
	
	for(int i = 0; i < broken_walls.size(); ++i)
	{
		float explosion_amplitude = A*(0.8*rand_interval()+0.2);

		broken_walls[i].initialize_explosion(source,explosion_amplitude,W);

		dust_particles[i].f_explosion = explosion_amplitude*normalize(-source)*pow(dust_particles[i].diametr,2);
	}
}

void scene_structure::draw_ground()
{
	for(int i = -10; i < 10; ++i)
	{
		for(int j = -10; j < 10; ++j)
		{
			ground.model.translation = vec3(i*20,0,j*20);
			draw(ground, environment);
		}
	}
}

void scene_structure::draw_broken_wall(broken_wall &b_wall, const vec3 &translation)
{
	for(int ind = 0; ind < b_wall.particles.size(); ++ind)
	{
		b_wall.meshes[ind].model = b_wall.particles[ind].mesh_transform;
		b_wall.meshes[ind].model.translation += translation;
		draw(b_wall.meshes[ind], environment);
	}
}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	timer.update(); // update the timer to the current elapsed time
	float const dt = 0.005f * timer.scale;

	//draw(skybox, environment); //skybox doesn't seem to be supported in this version of cgp
	draw_ground();
	draw(building, environment);
	
	//for transparent displaying
	glEnable(GL_BLEND);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//not explosive part
	for(int i = 0; i < walls_per_vertical; ++i)
	{
		for(int j = 0; j < walls_per_horizontal; ++j)
		{
			if((i == walls_per_vertical-1) && (j == walls_per_horizontal-1)) //no wall here
				continue;
			wall.model.rotation = rotation_transform();
			wall.model.translation = back_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
			draw(wall, environment);
		}
	}
	for(int i = 0; i < walls_per_vertical; ++i)
	{
		for(int j = 0; j < side_walls_per_horizontal; ++j)
		{
			wall.model.rotation =  rotation_transform::from_axis_angle(vec3(0,1,0),3.14159265/2);
			wall.model.translation = lside_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(0,0,2*Dx+d_z);
			draw(wall, environment);

			wall.model.translation = rside_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(0,0,2*Dx+d_z);
			draw(wall, environment);
		}
	}

	if(gui.explode)
	{
		//calculating simulation
		for(int i = 0; i < broken_walls.size(); ++i)
		{
			if(explosion_timer < explosion_period*i)
				continue;
			simulate_wall_explosion(dt, broken_walls[i].particles);
		}
		for(int i = 0; i < dust_particles.size(); ++i)
		{
			if(explosion_timer < explosion_period*i)
				continue;
			simulate_dust_explosion(dt,dust_particles[i]);
		}
		for(int i = 0; i < flame_particles.size(); ++i)
		{
			if(explosion_timer < explosion_period*i)
				continue;
			flame_particles[i].frame_timer += dt;
		}	
		explosion_timer += dt;

		//drawing
		for(int i = 0; i < flame_particles.size(); ++i)
		{
			if(explosion_timer < explosion_period*i)
				continue;
			if(i==3) //skip the door
				continue;
			int frame = (int)(flame_particles[i].frame_timer/flame_period);
			if(frame < flame_billboards.size())
			{
				flame_billboards[frame].model.translation = flame_particles[i].position;
				flame_billboards[frame].model.rotation = camera_control.camera_model.orientation();
				draw(flame_billboards[frame], environment);
			}
		}

		for(int i = 0; i < walls_per_vertical; ++i)
		{
			for(int j = 0; j < walls_per_horizontal; ++j)
			{
				if(explosion_timer < explosion_period*(i*walls_per_horizontal+j))
					continue;
				if((i == 0) && (j == 3)) //skip the door
					continue;
				vec3 translation = front_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
				draw_broken_wall(broken_walls[i*walls_per_horizontal+j],translation);
			}
		}

		for(int i = 0; i < dust_particles.size(); ++i)
		{
			if(explosion_timer < explosion_period*i)
				continue;
			if(i == 3) //skip the door
				continue;
			dust_billboard.model.rotation = camera_control.camera_model.orientation();
			dust_billboard.model.translation = dust_particles[i].p;
			draw(dust_billboard, environment);
		}

		for(int i = 0; i < walls_per_vertical; ++i)
		{
			for(int j = 0; j < walls_per_horizontal; ++j)
			{
				if(explosion_timer >= explosion_period*(i*walls_per_horizontal+j))
					continue;
				wall.model.rotation = rotation_transform();
				wall.model.translation = front_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
				draw(wall, environment);
			}
		}
	}
	else
	{
		for(int i = 0; i < walls_per_vertical; ++i)
		{
			for(int j = 0; j < walls_per_horizontal; ++j)
			{
				wall.model.rotation = rotation_transform();
				wall.model.translation = front_left_low_corner + i*vec3(0,2*Dy+d_y,0) + j*vec3(2*Dx+d_x,0,0);
				draw(wall, environment);
			}
		}
	}
	glDepthMask(true);
	glDisable(GL_BLEND);
}

void scene_structure::display_gui()
{
	ImGui::SliderFloat("Timer scale", &timer.scale, 0.01f, 4.0f, "%0.2f");

	bool const restart = ImGui::Button("Restart");
	if (restart)
	{
		initialize_broken_walls();
		initialize_dust();
		initialize_flame();
		initialize_explosion();
		explosion_timer=0;
	}

	ImGui::Checkbox("Explode", &gui.explode);
}

void scene_structure::mouse_move_event()
{	
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
		
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

