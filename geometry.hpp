#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "vec3.hpp"
#include <vector>
#define BOX_BIAS .01

enum vis_type {TRANSPARENT, OPAQUE, REFLECTIVE};

class color_info
{
    public:
        vec3 ka;
        vec3 kd;
        vec3 ks;
        vis_type vis; 
        float alpha;
        float ior;
};

const color_info DEFAULT_COLOR{vec3(.5, 0, 0), vec3(.7, .1, .1), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info RED{vec3(.5, 0, 0), vec3(.7, .1, .1), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info GREEN{vec3(0, .5, 0), vec3(.1, .7, .1), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info BLUE{vec3(0, 0, .7), vec3(.1, .1, .7), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info PURPLE{vec3(.5, 0, .5), vec3(.7, .1, .7), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info BLACK{vec3(), vec3(.1, .1, .1), 
vec3(1, 1, 1), OPAQUE, 100, 1};

const color_info MIRROR{vec3(), vec3(), vec3(), REFLECTIVE, 5, 1};

const color_info GLASS{vec3(), vec3(), vec3(), TRANSPARENT, 5, 1.5};

const color_info WATER{vec3(), vec3(), vec3(), TRANSPARENT, 5, 1.333};

class Shape 
{
    public:
        vec3 ka;
        vec3 kd;
        vec3 ks;
        vis_type vis; 
        float alpha;
        float ior;
        vec3 minBounds;
        vec3 maxBounds;
        // Random values for an opaque object 
        Shape()
        {
           ka = DEFAULT_COLOR.ka;
           kd = DEFAULT_COLOR.kd;
           ks = DEFAULT_COLOR.ks;
           vis = DEFAULT_COLOR.vis;
           alpha = DEFAULT_COLOR.alpha;
           ior = DEFAULT_COLOR.ior;
           minBounds = vec3();
           maxBounds = vec3();
        }
        virtual bool inBounds(vec3 min, vec3 max) const = 0;
        virtual std::pair<vec3, vec3> bounds() const = 0;
        virtual vec3 normal(vec3 point) const = 0;
        virtual float intersection(vec3 origin, vec3 direction) const = 0;
};

class Sphere: public Shape
{
    public:
        vec3 pos;
        float radius; 
        std::pair<vec3, vec3> sphere_bounds() const;
        Sphere()
        {
            pos = vec3(0, 0, 0);
            radius = 0;
        }

        Sphere(vec3 pos, float radius)
        {
            this->pos = pos;
            this->radius = radius;
            std::pair<vec3, vec3> bound = this->sphere_bounds();
            this->minBounds = bound.first;
            this->maxBounds = bound.second;
        }

        vec3 normal(vec3 point) const
        {
            return (point - this->pos).normalize();
        }
        float intersection(vec3 origin, vec3 direction) const;
        bool inBounds(vec3 min, vec3 max) const;
        std::pair<vec3, vec3> bounds() const;

};

class Triangle: public Shape
{
    public:
        vec3 v0;
        vec3 v1;
        vec3 v2;
        vec3 n;
        std::pair<vec3, vec3> triangle_bounds() const;
        Triangle()
        {
            v0 = vec3(0, 0, 0);
            v1 = vec3(0, 0, 0);
            v2 = vec3(0, 0, 0);
            n = vec3(0, 0, 0);
        }
        // Initialize vertices counterclockwise to get rhr normal
        Triangle(vec3 v0, vec3 v1, vec3 v2)
        {
            this->v0 = v0;
            this->v1 = v1;
            this->v2 = v2;
            this->n = ((v1 - v0).crossProduct(v2 - v0)).normalize();
            std::pair<vec3, vec3> bound = this->triangle_bounds();
            this->minBounds = bound.first;
            this->maxBounds = bound.second;
        }

        Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 normal)
        {
            this->v0 = v0;
            this->v1 = v1;
            this->v2 = v2;
            this->n = normal;
            std::pair<vec3, vec3> bound = this->triangle_bounds();
            this->minBounds = bound.first;
            this->maxBounds = bound.second;
        }

        vec3 normal(vec3 point) const
        {
            return this->n;
        }
        float intersection(vec3 origin, vec3 direction) const;
        bool inBounds(vec3 min, vec3 max) const;
        std::pair<vec3, vec3> bounds() const;
};

class Light
{
    public:
        vec3 ia;
        vec3 id;
        vec3 is;
        vec3 pos;

        Light(vec3 pos)
        {
            this->ia = vec3(.2, .2, .2);
            this->id = vec3(1, 1, 1);
            this->is = vec3(.5, .5, .5);
            this->pos = pos;
        }
        Light(vec3 ia, vec3 id, vec3 is, vec3 pos)
        {
            this->ia = ia;
            this->id = id;
            this->is = is;
            this->pos = pos;
        }
};
std::pair<float, float> aabb_intersection(vec3 minBounds, vec3 maxBounds, 
vec3 origin, vec3 dir);
#endif
