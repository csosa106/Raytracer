#include "raytracer.hpp"
#include "geometry.hpp"
#include "vec3.hpp"
#include "drawer.hpp"
#include "loader.hpp"
#include "kdtree.hpp"
#include <iostream>

int main(void)
{
    // Vector containing primitives
    std::vector<Shape*> scenery;

    // .obj files
    std::string object_path = "dragon.obj";
    vec3 object_pos = vec3(0, 0, 0);
    vec3 object_scale(1, 1, 1);
    int num_tris = num_verts(object_path) / 3;
    Triangle *obj_ptr = new Triangle[num_tris];
    load_and_add(object_path, obj_ptr, num_tris, scenery, object_pos, 
    object_scale, GREEN);

    // Spheres
    Sphere s(vec3(1, 0, -1), .1);
    s.vis = REFLECTIVE;
    //scenery.push_back(&s);


    // Room
    Triangle *room_ptr = new Triangle[12];
    std::vector<color_info> info = {MIRROR, RED, GREEN, PURPLE, BLUE, BLACK};
    vec3 room_extent = vec3(9.5, 9.5, 9.5);
    vec3 room_center = vec3();
    aa_room(room_center, room_extent, room_ptr, scenery, info);

    // Bounds
    vec3 minBounds = vec3(-10, -10, -10);
    vec3 maxBounds = -minBounds;

    // kd tree
    std::cout << "Creating kd tree" << std::endl;
    int leaf_size = 10;
    int tree_max_depth = 40;
    kdtree t(minBounds, maxBounds, scenery, leaf_size, tree_max_depth);
    std::cout << "Finished creating kd tree" << std::endl;
    
    // Camera
    vec3 camera_pos = vec3(5, 0, -5);
    vec3 center = vec3();
    vec3 up = vec3(0, 1, 0);

    //Lights
    std::vector<Light> lights;
    lights.push_back(Light(vec3(0, 9, 0)));

    // Rendering
    auto rgb = new float [HEIGHT][WIDTH][3];
    draw(t, lights, camera_pos, center, up, HEIGHT, WIDTH, IOR, 0, BIAS, FOV, 
    "name", rgb);
    std::cout << "Finished rendering" << std::endl;
    delete rgb;
    delete obj_ptr;
    delete room_ptr;
    return 0;
}
