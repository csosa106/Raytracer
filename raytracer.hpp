#ifndef RAYTRACER_H
#define RAYTRACER_H
#define IOR 1.0
#define WIDTH 500
#define HEIGHT 500
#define MAX_DEPTH 9
#define BIAS .01
#define FOV 50
#include "vec3.hpp"
#include "geometry.hpp"
#include "kdtree.hpp"
#include <memory>

class NodeStack
{
    public:
        std::shared_ptr<Node> n;
        float t_min;
        float t_max;
};

const vec3 BG_COLOR = vec3(.2, .2, .2);

bool inShadow(vec3 point, const kdtree &t, Light l);

vec3 lighting(vec3 origin, vec3 direction, std::pair<float, Shape*> hit, 
const kdtree &t, const std::vector<Light> &lights, int depth);

vec3 primary(int row, int col, float fov);

std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> order(vec3 dir, 
int axis, std::shared_ptr<Node> n);

std::pair<float, Shape*> searchNode(std::shared_ptr<Node> n, vec3 origin, 
vec3 direction, float t_min, float t_max, std::vector<NodeStack> &s);

std::pair<float, Shape*> closestIntersect(vec3 origin, vec3 direction, 
const kdtree &t);

vec3 caster(vec3 origin, vec3 direction, const kdtree &t, 

const std::vector<Light> &lights, int depth);

float* index(float *p, int i, int j);

float* lookAtMatrix(vec3 eye, vec3 center, vec3 up);

vec3 lookAt(float *mat, vec3 v);

void sphere_populator(vec3 minBounds, vec3 maxBounds, int num_sphere, 
float max_rad, Sphere *s, std::vector<Shape*> &scenery);

std::vector<Triangle> panel_placer(vec3 center, vec3 extent, color_info info);

void aa_room(vec3 center, vec3 extent, Triangle *t, 
std::vector<Shape*> &scenery, std::vector<color_info> info);
#endif
