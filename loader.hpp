#ifndef LOADER_H
#define LOADER_H
#include "geometry.hpp"
#include "raytracer.hpp"
#include "vec3.hpp"
#include <string>
#include <vector>
int num_verts(std::string path);

void tri_ptr(std::string path, Triangle *t, color_info info);

void add(Triangle* t, std::vector<Shape*> &scenery, int size, vec3 pos, 
vec3 scaler);
std::vector<vec3> extent(std::string path);

vec3 center_obj(std::string path, float mult);

void load_and_add(std::string path, Triangle *t, int num_tris, 
std::vector<Shape*> &scenery, vec3 pos, vec3 scaler, color_info info);

std::pair<vec3, vec3> scene_extent(const std::vector<Shape*> &scenery);
#endif
