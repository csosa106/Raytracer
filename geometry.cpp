#include <cmath>
#include <algorithm>
#include "geometry.hpp"

// https://gamedev.stackexchange.com/questions/18436/
// most-efficient-aabb-vs-ray-collision-algorithms
std::pair<float, float> aabb_intersection(vec3 minBounds, vec3 maxBounds, 
vec3 origin, vec3 dir)
{
    float inverse_x = 1.0 / dir.coord[0];
    float inverse_y = 1.0 / dir.coord[1];
    float inverse_z = 1.0 / dir.coord[2];

    float t1 = (minBounds.coord[0] - origin.coord[0]) * inverse_x;
    float t2 = (maxBounds.coord[0] - origin.coord[0]) * inverse_x;

    float t3 = (minBounds.coord[1] - origin.coord[1]) * inverse_y;
    float t4 = (maxBounds.coord[1] - origin.coord[1]) * inverse_y;

    float t5 = (minBounds.coord[2] - origin.coord[2]) * inverse_z;
    float t6 = (maxBounds.coord[2] - origin.coord[2]) * inverse_z;

    float t_min = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), 
    std::min(t5, t6)); 
    float t_max = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), 
    std::max(t5, t6));
    std::pair<float, float> result;
    if (t_min > t_max)
    {
        result.first = INFINITY;
        result.second = INFINITY;
    }
    result.first = t_min;
    result.second = t_max;
    return result;
}


float Sphere::intersection(vec3 origin, vec3 direction) const
{
    float ans;
    float discriminant = pow(direction * (origin - this->pos), 2.0) - 
    (pow((origin - this->pos).norm(), 2.0)- pow(radius, 2.0));
    if (discriminant < 0)
    {
        ans = INFINITY;
    }
    else if (discriminant == 0)
    {
        float neg_term = -(direction * (origin - this->pos));
        ans = neg_term;
    }
    else
    {
        float neg_term = -(direction * (origin - this->pos));
        // Discard negative solutions, return smallest positive answer
        float pos_answer = neg_term + pow(discriminant, .5);
        float neg_answer = neg_term - pow(discriminant, .5);
        if (neg_answer > 0)
        {  
            ans = neg_answer;
        }
        else
        {
            ans = pos_answer > 0 ? pos_answer : INFINITY;
        }
    }
    return ans;
}

// https://blackpawn.com/texts/pointinpoly/
float Triangle::intersection(vec3 origin, vec3 direction) const
{
    // Point on plane containing triangle that ray intersects with
    float dist = ((this->v0 - origin) * this->n) / (this->n * direction);
    if (dist < 0)
    {
        return INFINITY;
    }
    vec3 point = origin + dist * direction;

    // Determining if point is in triangle
    vec3 p0 = v2 - v0;
    vec3 p1 = v1 - v0;
    vec3 p2 = point - v0;

    float dot00 = p0 * p0;
    float dot01 = p0 * p1;
    float dot02 = p0 * p2;
    float dot11 = p1 * p1;
    float dot12 = p1 * p2;

    float denom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * denom;
    float v = (dot00 * dot12 - dot01 * dot02) * denom;
    
    
    if ((u >= 0) && (v >= 0) && (u + v < 1))
    {
        return dist;
    }
    else
    {
        return INFINITY;
    }
}


std::pair<vec3, vec3> Sphere::sphere_bounds() const
{
    std::pair<vec3, vec3> result;
    result.first = pos + vec3(-radius, -radius, -radius);
    result.second = pos + vec3(radius, radius, radius);
    return result;
}

std::pair<vec3, vec3> Triangle::triangle_bounds() const
{
    vec3 total_min = vec3(INFINITY, INFINITY, INFINITY);
    vec3 total_max = -total_min;
    vec3 vertices[3] = {v0, v1, v2};
    std::pair<vec3, vec3> result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (vertices[i].coord[j] > total_max.coord[j])
            {
                total_max.coord[j] = vertices[i].coord[j];
            }
            if (vertices[i].coord[j] < total_min.coord[j])
            {
                total_min.coord[j] = vertices[i].coord[j];
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (total_max.coord[i] == total_min.coord[i])
        {
            total_max.coord[i] += BOX_BIAS;
            total_min.coord[i] -= BOX_BIAS;

        }
    }
    result.first = total_min;
    result.second = total_max;
    return result;
}

bool Sphere::inBounds(vec3 min, vec3 max) const
{
    float dmin = 0;
    for (int i = 0; i < 3; i++)
    {
        if (pos.coord[i] < min.coord[i])
        {
            dmin += pow(pos.coord[i] - min.coord[i], 2);
        }
        else if (pos.coord[i] > max.coord[i])
        {
            dmin += pow(pos.coord[i] - max.coord[i], 2);
        }
    }
    return (dmin <= radius * radius);
}

// https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/
// code/tribox_tam.pdf
bool Triangle::inBounds(vec3 min, vec3 max) const
{
    vec3 box_center = .5 * (min + max);
    vec3 half_length = vec3(max.coord[0] - box_center.coord[0], 
    max.coord[1] - box_center.coord[1], max.coord[2] - box_center.coord[2]);
    vec3 v0_shift = v0 - box_center;
    vec3 v1_shift = v1 - box_center;
    vec3 v2_shift = v2 - box_center;
    vec3 f0 = v1_shift - v0_shift;
    vec3 f1 = v2_shift - v1_shift;
    vec3 f2 = v0_shift - v2_shift;
    vec3 axes[13];

    vec3 e0 = vec3(1, 0, 0);
    vec3 e1 = vec3(0, 1, 0);
    vec3 e2 = vec3(0, 0, 1);
    axes[0] = e0;
    axes[1] = e1;
    axes[2] = e2;
    axes[3] = n;

    axes[4] = e0.crossProduct(f0);
    axes[5] = e0.crossProduct(f1);
    axes[6] = e0.crossProduct(f2);
    
    axes[7] = e1.crossProduct(f0);
    axes[8] = e1.crossProduct(f1);
    axes[9] = e1.crossProduct(f2);

    axes[10] = e2.crossProduct(f0);
    axes[11] = e2.crossProduct(f1);
    axes[12] = e2.crossProduct(f2);

    for (int i = 0; i < 13; i++)
    {
        float p0 = v0_shift * axes[i];
        float p1 = v1_shift * axes[i];
        float p2 = v2_shift * axes[i];
        float r = half_length.coord[0] * fabsf(axes[i].coord[0]) +
        half_length.coord[1] * fabsf(axes[i].coord[1]) + 
        half_length.coord[2] * fabsf(axes[i].coord[2]);
        float max_point = std::max(p0, std::max(p1, p2));
        float min_point = std::min(p0, std::min(p1, p2));
        if (std::max(-max_point, min_point) > r)
        {
            return false;
        }
    }
    return true;
}

std::pair<vec3, vec3> Triangle::bounds() const
{
    std::pair<vec3, vec3> result;
    result.first = minBounds;
    result.second = maxBounds;
    return result;
}

std::pair<vec3, vec3> Sphere::bounds() const
{
    std::pair<vec3, vec3> result;
    result.first = minBounds;
    result.second = maxBounds;
    return result;
}

