#include "geometry.hpp"
#include "vec3.hpp"
#include "raytracer.hpp"
#include <algorithm>

bool inShadow(vec3 point, const kdtree &t, Light l)
{
    vec3 dir = (l.pos - point).normalize();
    std::pair<float, Shape*> result = closestIntersect(point, dir, t);
    return result.first < (l.pos - point).norm();
}

// Phong illumination model
vec3 lighting(vec3 origin, vec3 direction, std::pair<float, Shape*> hit, 
const kdtree &t, const std::vector<Light> &lights, int depth)
{
    vec3 result = vec3();
    vec3 hitPoint = origin + hit.first * direction;
    vis_type lighting_type = hit.second->vis;
    vec3 n = hit.second->normal(hitPoint);

    if (lighting_type == OPAQUE)
    {
        for (unsigned int i = 0; i < lights.size(); i++)
        {
            Light light_i = lights[i];
            vec3 ka = hit.second->ka;
            result += ka.comp_mul(light_i.ia);
            bool shadow_test = !inShadow(hitPoint + BIAS * n, t, light_i);

            if (shadow_test)
            {
                vec3 kd = (hit.second)->kd;
                vec3 ks = (hit.second)->ks;
                float alpha = (hit.second)->alpha;
                vec3 to_l = (light_i.pos - hitPoint).normalize();
                vec3 to_v = -direction;
                vec3 r = (-to_l).reflect(n);

                result += fmax(0, n * to_l) * kd.comp_mul(light_i.id) + 
                pow(fmax(0, r * to_v), alpha) * ks.comp_mul(light_i.is);
            }
        }
    }
    else if (lighting_type == REFLECTIVE)
    {
        result = caster(hitPoint + (BIAS * n), 
        direction.reflect(n), t, lights, depth + 1);
    }
    else
    {
        float ior1 = IOR;
        float ior2 = hit.second->ior;
        // Swap indices of refraction if ray is leaving object
        if (direction * n > 0)
        {
            ior1 = hit.second->ior;
            ior2 = IOR;
            n = -n;
        }
        vec3 refract_dir = direction.refract(n, ior1, ior2);
        vec3 reflect_dir = direction.reflect(n);
        if (refract_dir == vec3(0, 0, 0))
        {
            result = caster(hitPoint + BIAS * n,
            direction.reflect(n), t, lights, depth + 1);
        }
        else
        {
            vec3 reflect_result = caster(hitPoint + (BIAS * n),
            reflect_dir, t, lights, depth + 1);
            vec3 refract_result = caster(hitPoint - (BIAS * n),
            refract_dir, t, lights, depth + 1);
            float trans = fresnel(n, direction, ior1, ior2);
            result = (1.0 - trans) * reflect_result +  trans * refract_result;
        }
    }
    return result.clamp();
}

// Returns point on screen that ray emitted at the origin to pixel (row, col)
// would intersect with
vec3 primary(int row, int col, float fov)
{
    float ratio = WIDTH / HEIGHT;
    float py = (1 - 2 * ((row + .5) / HEIGHT)) * tan(fov / 2.0 * M_PI / 180);
    float px = (2 * ((col + .5) / WIDTH) - 1) * tan(fov / 2.0 * M_PI / 180) *
    ratio;
    vec3 primary = vec3(px, py, -1);
    return primary;
}

std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> order(vec3 dir, 
int axis, std::shared_ptr<Node> n)
{
    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> result;
    if (dir.coord[axis] > 0)
    {
        result.first = n->left;
        result.second = n->right;
    }
    else
    {
        result.first = n->right;
        result.second = n->left;
    }
    return result;
}

std::pair<float, Shape*> searchNode(std::shared_ptr<Node> n, vec3 origin, 
vec3 direction, float t_min, float t_max, std::vector<NodeStack> &s)
{
    std::pair<float, Shape*> result;
    result.first = INFINITY;
    result.second = NULL;
    if (n->isLeaf)
    {
        for (unsigned int i = 0; i < (n->primitives).size(); i++)
        {
            float intersect = ((n->primitives)[i])->
            intersection(origin, direction);
            if (intersect < result.first)
            {
                result.first = intersect;
                result.second = (n->primitives)[i];
            }
        }
        if (result.first < t_max)
        {
            return result;
        }
        else if (s.empty())
        {
            result.first = INFINITY;
            result.second = NULL;
            return result;
        }
        else
        {
            NodeStack popped = s.back();
            s.pop_back();
            return searchNode(popped.n, origin, direction, popped.t_min, 
            popped.t_max, s);
        }

    }
    else
    {
        // Coordinate of split is on the split_axis dimension and equal to 
        // the maxBounds of the left node or minBound of right node
        float split_val = ((n->left)->maxBounds).coord[n->split_axis];
        float inverse_coord = 1.0 / direction.coord[n->split_axis];
        float t_hit = (split_val - origin.coord[n->split_axis]) * inverse_coord;

        std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> ordered = 
        order(direction, n->split_axis, n);

        if (t_hit < t_min)
        {

            return searchNode(ordered.second, origin, direction, t_min, t_max, 
            s);
        }
        else if (t_hit > t_max)
        {
            return searchNode(ordered.first, origin, direction, t_min, t_max, 
            s);
        }
        else
        {
            NodeStack pushed;
            pushed.n = ordered.second;
            pushed.t_min = t_hit;
            pushed.t_max = t_max;
            s.push_back(pushed);
            return searchNode(ordered.first, origin, direction, t_min, t_hit, 
            s);
        }
    }
}

std::pair<float, Shape*> closestIntersect(vec3 origin, vec3 direction, 
const kdtree &t)
{
    std::pair<float, float> t_bounds = 
    aabb_intersection((t.root)->minBounds, (t.root)->maxBounds,
    origin, direction);
    std::vector<NodeStack> s;
    std::pair<float, Shape*> result;
    if (std::isinf(t_bounds.first))
    {
        result.first = INFINITY;
    }
    else
    {
        result = searchNode(t.root, origin, direction, t_bounds.first, 
        t_bounds.second, s);
    }
    s.clear();
    return result;
}


// Returns the rgb data a observer at vec3 origin would see when looking in 
// the direction of vec3 direction
vec3 caster(vec3 origin, vec3 direction, const kdtree &t, 
const std::vector<Light> &lights, int depth)
{
    if (depth > MAX_DEPTH)
    {
        return BG_COLOR;
    }
    std::pair<float, Shape*> hit = closestIntersect(origin, direction, t);
    if (hit.first < INFINITY)
    {
        vec3 lighting_val = lighting(origin, direction, hit, t, lights, depth);
        return lighting_val;
    }
    else
    {
        return BG_COLOR;
    }
}

float* index(float *p, int i, int j)
{
    return (p + (i * 4 + j));
    
}

// https://stackoverflow.com/questions/19740463/lookat-function-im-going-crazy
float* lookAtMatrix(vec3 eye, vec3 center, vec3 up)
{
    vec3 z = (center - eye).normalize();
    vec3 x = (z.crossProduct(up.normalize())).normalize();
    vec3 y = x.crossProduct(z);

    float* mat = new float[16];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            *(index(mat, i, j)) = 0.0;
        }
    }
    *(index(mat, 3, 3)) = 1.0;
    *(index(mat, 0, 0)) = x.coord[0]; 
    *(index(mat, 1, 0)) = x.coord[1];
    *(index(mat, 2, 0)) = x.coord[2];
    *(index(mat, 0, 1)) = y.coord[0];
    *(index(mat, 1, 1)) = y.coord[1];
    *(index(mat, 2, 1)) = y.coord[2];
    *(index(mat, 0, 2)) = -z.coord[0];
    *(index(mat, 1, 2)) = -z.coord[1];
    *(index(mat, 2, 2)) = -z.coord[2];
    *(index(mat, 3, 0)) = -(x * eye);
    *(index(mat, 3, 1)) = -(y * eye);
    *(index(mat, 3, 2)) = (z * eye);
    return mat;    
}

vec3 lookAt(float *mat, vec3 v)
{
    vec3 result;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.coord[i] +=  *(index(mat, i, j)) * v.coord[j];
        }
    }
    return result;
}

void sphere_populator(vec3 minBounds, vec3 maxBounds, int num_sphere, 
float max_rad, Sphere *s, std::vector<Shape*> &scenery)
{
    for (int i = 0; i < num_sphere; i++)
    {
        float rand_r = max_rad * ((float) rand() / RAND_MAX);
        float rand_x = (maxBounds.coord[0] - minBounds.coord[0]) * 
        ((float) rand() / RAND_MAX) + minBounds.coord[0];
        float rand_y = (maxBounds.coord[1] - minBounds.coord[1]) * 
        ((float) rand() / RAND_MAX) + minBounds.coord[1];
        float rand_z = (maxBounds.coord[2] - minBounds.coord[2]) * 
        ((float) rand() / RAND_MAX) + minBounds.coord[2];
        *(s+i) = Sphere(vec3(rand_x, rand_y, rand_z), rand_r);
        scenery.push_back((s+i));
    }
}

std::vector<Triangle> panel_placer(vec3 center, vec3 extent, color_info info)
{
    vec3 top_right = center;
    vec3 bottom_left = center;
    vec3 normal = (-center).normalize();
    std::vector<Triangle> result;

    std::vector<int> used;
    for (int i = 0; i < 3; i++)
    {
        if (extent.coord[i] != 0)
        {
            top_right.coord[i] = center.coord[i] + extent.coord[i];
            bottom_left.coord[i] = center.coord[i] - extent.coord[i];
            used.push_back(i);
        }
    }
    vec3 corner1 = top_right;
    vec3 corner2 = top_right;
    corner1.coord[used[0]] = center.coord[used[0]] - extent.coord[used[0]];
    corner2.coord[used[1]] = center.coord[used[1]] - extent.coord[used[1]];
    Triangle t1 = Triangle(bottom_left, top_right, corner1, normal);
    Triangle t2 = Triangle(bottom_left, top_right, corner2, normal);
    t1.ka = info.ka;
    t1.kd = info.kd;
    t1.ks = info.ks;
    t1.vis = info.vis;
    t1.alpha = info.alpha;
    t1.ior = info.ior;
    t2.ka = info.ka;
    t2.kd = info.kd;
    t2.ks = info.ks;
    t2.vis = info.vis;
    t2.alpha = info.alpha;
    t2.ior = info.ior;
    result.push_back(t1);
    result.push_back(t2);
    return result;

}

// Room colors go as +x -x +y -y +z -z
void aa_room(vec3 center, vec3 extent, Triangle *t, 
std::vector<Shape*> &scenery, std::vector<color_info> info)
{
    vec3 extent_x = extent;
    vec3 extent_y = extent;
    vec3 extent_z = extent;

    extent_x.coord[0] = 0;
    extent_y.coord[1] = 0;
    extent_z.coord[2] = 0;

    vec3 pos_x_center = center;
    vec3 pos_y_center = center;
    vec3 pos_z_center = center;
    pos_x_center.coord[0] = center.coord[0] + extent.coord[0];
    pos_y_center.coord[1] = center.coord[1] + extent.coord[1];
    pos_z_center.coord[2] = center.coord[2] + extent.coord[2];

    vec3 neg_x_center = center;
    vec3 neg_y_center = center;
    vec3 neg_z_center = center;
    neg_x_center.coord[0] = center.coord[0] - extent.coord[0];
    neg_y_center.coord[1] = center.coord[1] - extent.coord[1];
    neg_z_center.coord[2] = center.coord[2] - extent.coord[2];

    std::vector<Triangle> positive_x = panel_placer(pos_x_center, extent_x, 
    info[0]);
    std::vector<Triangle> positive_y = panel_placer(pos_y_center, extent_y, 
    info[2]);
    std::vector<Triangle> positive_z = panel_placer(pos_z_center, extent_z, 
    info[4]);

    std::vector<Triangle> negative_x = panel_placer(neg_x_center, extent_x, 
    info[1]);
    std::vector<Triangle> negative_y = panel_placer(neg_y_center, extent_y, 
    info[3]);
    std::vector<Triangle> negative_z = panel_placer(neg_z_center, extent_z, 
    info[5]);

    std::vector<std::vector<Triangle>> tris = {positive_x, positive_y, 
    positive_z, negative_x, negative_y, negative_z};
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            *(t + i * 2 + j) = tris[i][j];
            scenery.push_back((t + i * 2 + j));
        }
    }
}
