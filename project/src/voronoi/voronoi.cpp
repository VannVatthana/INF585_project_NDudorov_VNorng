#include "voronoi.hpp"



vec3 compute_centroid(const numarray<vec3> &points)
{
    vec3 centroid(0,0,0);
    for(const vec3 &point : points)
    {
        centroid += point;
    }
    return centroid/points.size();
}

float compute_diametr(const numarray<vec3> &points)
{
    float diametr = 0;
    for(const vec3 &point : points)
    {
        diametr += norm(point);
    }
    return 2*diametr/points.size();
}

//read file storing voronoi cell data in format: 
//face vertex coordinates, face vertex indices
//vertices: (x1,y1,z1), (x2,y2,z2), ..., (xn,yn,zn)\n
//faces: (v1,v2,...,vk1), (v1,v2,...,vk2), ..., (v1,v2,...,vkn)\n
void read_voronoi(std::vector<voronoi_particle> &particles, std::vector<mesh_drawable> &cells, const std::string &path_to_diagram)
{
    if((cells.size()!=0) || (particles.size()!=0))
    {
        throw std::runtime_error("not an empty vector passed to initialize voronoi cells");
    }
    std::ifstream ifs(path_to_diagram);
    unsigned int cell_id=0;

    while(!ifs.eof())
    {
        std::string line_site,line_vertices,line_faces; 
        std::getline(ifs, line_vertices);
        std::getline(ifs,line_faces);
        std::stringstream  stream_vertices(line_vertices);
        std::stringstream  stream_faces(line_faces);

        numarray<vec3> vertices;
        numarray<uint3> faces;
        char separ;
        float x,y,z;
        unsigned int v;

        while(stream_vertices >> separ >> x >> separ >> y >> separ >> z >> separ)
        {
            vertices.push_back(vec3(x,y,z));
        }
        vec3 centroid = compute_centroid(vertices);
        for(vec3 &vertex: vertices)
        {
            vertex = vertex - centroid; //coordinates with respect to centroid of the cell
        }
        float diametr = compute_diametr(vertices);
    
        while(!stream_faces.eof())
        {
            numarray<unsigned int> polygon;
            stream_faces >> separ;
            while((stream_faces.peek() != ' ') && (!stream_faces.eof()))
            {
                stream_faces >> v >> separ;
                polygon.push_back(v);
            }

            for(int v=1;v<polygon.size()-1;++v) //divide polygon on triangles
            {
                faces.push_back({polygon[0],polygon[v],polygon[v+1]});
            } 

            stream_faces.get(); //skip ' ' space
        }
    
        mesh cell_mesh;
        mesh_drawable cell;
        cell_mesh.position=vertices;
        cell_mesh.connectivity=faces;
        cell_mesh.flip_connectivity();
        cell_mesh.fill_empty_field();
        cell.initialize_data_on_gpu(cell_mesh);
        cells.push_back(cell);
        voronoi_particle particle = voronoi_particle(diametr,centroid);
        particles.push_back(particle);
        cell_id+=1;

        ifs.get(); //skip empty line;
    }
}

