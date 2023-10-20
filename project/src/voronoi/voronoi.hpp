#pragma once

#include <fstream>
#include <vector>
#include "cgp/cgp.hpp"

using namespace cgp;

struct voronoi_particle
{
    voronoi_particle(float diam, const vec3 &position)
    {
        diametr = diam;
        m = density * pow(diametr,3);
        transform(0,position);
    }
    float density = 1.;
    float diametr = 1.;
    vec3 centroid = vec3(0.,0.,0.); //centroid of the voronoi cell
    vec3 angular_v = vec3(0.,0.,1);
    float angle = 0.;
    vec3 v = vec3(0.,0.,0.);
    float m = 1.;
    vec3 f_explosion = vec3(0.,0.,0.);
    affine_rt mesh_transform;

    void transform(float rot_angle, const vec3 &translation)
    {
        mesh_transform = affine_rt(rotation_transform(),translation)*
                      affine_rt(rotation_transform::from_axis_angle(normalize(angular_v),rot_angle));
        angle = rot_angle;
        centroid = translation;
    }
};

void read_voronoi(std::vector<voronoi_particle> &particles, std::vector<mesh_drawable> &cells, const std::string &path_to_diagram);