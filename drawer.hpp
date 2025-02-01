#ifndef DRAW_H
#define DRAW_H
#include "geometry.hpp"
#include "vec3.hpp"
#include "raytracer.hpp"
#include "kdtree.hpp"
#include <vector>
#include <string>
#include<fstream>

void draw(const kdtree &t, std::vector<Light> &lights, vec3 eye, vec3 center, 
vec3 up, int rows, int cols, float ior, int depth, float bias, float fov, 
std::string filename, float rgb[HEIGHT][WIDTH][3]);
#endif
