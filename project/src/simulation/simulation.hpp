#pragma once

#include "cgp/cgp.hpp"
#include "voronoi/voronoi.hpp"
#include "environment.hpp"
#include <vector>

struct dust_particle
{
    dust_particle(float diam, const vec3 &position)
    {
        diametr = diam;
        m = density * pow(diametr,3);
        p = position;
    }
    float density = 1.;
    float diametr = 1.;
    vec3 p = vec3(0.,0.,0.); //position of the partcile
    vec3 v = vec3(0.,0.,0.); //velocity of the particle
    float m = 1; //mass of the particle
    vec3 f_explosion = vec3(0.,0.,0.);
    float lifetime = 0.;
};

struct broken_wall
{
    std::vector<cgp::mesh_drawable> meshes; // meshes to display exploded wall piecies
	std::vector<voronoi_particle> particles; // exploded wall piecies

    void initialize()
    {
        particles.clear();
        meshes.clear();

        std::string rand_ind = std::to_string((int)(5*rand_interval())+1);
        read_voronoi(particles,meshes,project::path+"data/voronoi_diagram"+rand_ind+".custom"); //initialize cgp::mesh_drawable's with precomputed voronoi cells
    }

    void initialize_explosion(const vec3 &source, float explosion_amplitude, float W)
    {
        for(voronoi_particle &particle : particles)
	    {
            particle.f_explosion = explosion_amplitude*normalize(-source+particle.centroid)*pow(particle.diametr,2);
            particle.angular_v = W*normalize(0.001*vec3(1,1,1) + vec3(rand_interval(),rand_interval(),rand_interval()));
	    }
    }
};

struct flame_particle
{
    float frame_timer = 0;
    vec3 position = vec3(0,0,0);
};

void simulate_wall_explosion(float dt, std::vector<voronoi_particle>& particles);
void simulate_dust_explosion(float dt, dust_particle &particle);