#include "simulation.hpp"

using namespace cgp;


void simulate_wall_explosion(float dt, std::vector<voronoi_particle>& particles)
{
	// Numerical integration
	float const damping = 0.005f;
    vec3 g(0,-9.81f,0);
    float v_12 = 0.005;
    float alpha_v2 = 0.001;
    float alpha_v1 = 0.01;
    float alpha_angular_v = 0.99;
    float exp = 2.718281828;
    float gamma = 100.;

    int max_iter = 1;
    dt = dt/max_iter;
	for(voronoi_particle &cell: particles)
	{
        for(int iter = 0; iter < max_iter; ++iter)
        {
            vec3 f_friction = norm(cell.v) > v_12 ? -alpha_v2*normalize(cell.v)*dot(cell.v,cell.v) : -alpha_v1*cell.v;
            f_friction *= pow(cell.diametr,2);
            vec3 f_gravity = cell.m * g; 
            vec3 f = f_gravity + f_friction + cell.f_explosion;

            cell.v = (1-damping)*cell.v + dt*f/cell.m;
            vec3 position = cell.centroid + dt*cell.v;
            float rotation_angle = norm(cell.angular_v) > damping ? cell.angle+norm(cell.angular_v)*dt : cell.angle; 
            cell.transform(rotation_angle, position);

            cell.f_explosion = cell.f_explosion * pow(exp,-gamma*dt);
            cell.angular_v = norm(cell.angular_v) > damping ? alpha_angular_v*cell.angular_v : cell.angular_v;
        }
	}
}

void simulate_dust_explosion(float dt, dust_particle &particle)
{
	// Numerical integration 
	float const damping = 0.005f;
    vec3 g(0,-9.81/10,0); //less gravitation so that dust drops slowly
    float v_12 = 0.005;
    float alpha_v2 = 0.02;
    float alpha_v1 = 0.01;
    float exp = 2.718281828;
    float gamma = 100.;
    //float max_lifetime = 0.2;

    int max_iter = 2;
    dt = dt/max_iter;
    for(int iter=0;iter<max_iter;++iter)
    {   
        vec3 f_friction = norm(particle.v) > v_12 ? -alpha_v2*normalize(particle.v)*dot(particle.v,particle.v) : -alpha_v1*particle.v;
        f_friction *= pow(particle.diametr,2);
        vec3 f_gravity = particle.m * g; 
        vec3 f = f_gravity + f_friction + particle.f_explosion;

        particle.v = (1-damping)*particle.v + dt*f/particle.m;
        particle.p = particle.p + dt*particle.v;

        particle.f_explosion = particle.f_explosion * pow(exp,-gamma*dt); 
        //particle.lifetime += dt;
    }
}