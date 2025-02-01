#include "loader.hpp"
#include "obj_loader.hpp"

int num_verts(std::string path)
{
    objl::Loader Loader;
    bool loadout = Loader.LoadFile(path);
    int result = 0;
    if (loadout)
    {
        for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
        {
            objl::Mesh curMesh = Loader.LoadedMeshes[i];
            for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
            {
                result += 1;
            }
        }
    }
    return result;
}

void tri_ptr(std::string path, Triangle *t, color_info info)
{
    vec3 vertices[3];
    
    // Initialize Loader
    objl::Loader Loader;

    // Load .obj File
    bool loadout = Loader.LoadFile(path);

    if (loadout)
    {
        int offset = 0;
        for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
        {
            objl::Mesh curMesh = Loader.LoadedMeshes[i];
            
            for (unsigned int j = 0; j < curMesh.Vertices.size(); j+= 3)
            {
                for (int k = 0; k < 3; k++)
                {
                    vertices[k] = vec3(curMesh.Vertices[j+k].Position.X,
                    curMesh.Vertices[j+k].Position.Y, 
                    curMesh.Vertices[j+k].Position.Z);
                }
                *(t + offset) = Triangle(vertices[0], vertices[1], vertices[2]);
                (t + offset)->ka = info.ka;
                (t + offset)->kd = info.kd;
                (t + offset)->ks = info.ks;
                (t + offset)->vis = info.vis;
                (t + offset)->alpha = info.alpha;
                (t + offset)->ior = info.ior;
                offset += 1;
            }
        }
    }
}

void add(Triangle* t, std::vector<Shape*> &scenery, int size, vec3 pos, 
vec3 scaler)
{
    vec3 com;
    for (int i = 0; i < size; i++)
    {
        com += (t+i)->v0;
        com += (t+i)->v1;
        com += (t+i)->v2;
    }
    com = (1.0 / (3 * size)) * com;
    
    for (int i = 0; i < size; i++)
    {
        (t+i)->v0 = ((t+i)->v0 - com).comp_mul(scaler) + pos;
        (t+i)->v1 = ((t+i)->v1 - com).comp_mul(scaler) + pos;
        (t+i)->v2 = ((t+i)->v2 - com).comp_mul(scaler) + pos;
        (t+i)->n = (((t+i)->v1 - (t+i)->v0).crossProduct((t+i)->v2 - (t+i)->v0))
        .normalize();
        scenery.push_back((t+i));
    }
}

std::vector<vec3> extent(std::string path)
{
    std::vector<vec3> result;

    int num_tris = num_verts(path) / 3;
    Triangle *t = new Triangle[num_tris];
    tri_ptr(path, t, DEFAULT_COLOR);
    
    vec3 min_coord = vec3(INFINITY, INFINITY, INFINITY);
    vec3 max_coord = -min_coord;
    vec3 com;
    for (int i = 0; i < num_tris; i++)
    {
        std::pair<vec3, vec3> obj_bounds = (t+i)->bounds();
        com += (t+i)->v0;
        com += (t+i)->v1;
        com += (t+i)->v2;
        for (int j = 0; j < 3; j++)
        {
            if (obj_bounds.first.coord[j] < min_coord.coord[j])
            {
                min_coord.coord[j] = obj_bounds.first.coord[j];
            }
            if (obj_bounds.second.coord[j] > max_coord.coord[j])
            {
                max_coord.coord[j] = obj_bounds.second.coord[j];
            }
        }
    }
    com = (1.0 / (3 * num_tris)) * com;
    for (int i = 0; i < 3; i++)
    {
        if (min_coord.coord[i] == max_coord.coord[i])
        {
            min_coord.coord[i] -= BOX_BIAS;
            max_coord.coord[i] += BOX_BIAS;
        }
    }
    result.push_back(com);
    result.push_back(min_coord);
    result.push_back(max_coord);
    delete t;
    return result;
}

// Centers object so it fits in the camera perfectly at (0, 0, -c)
// when mult is 1
vec3 center_obj(std::string path, float mult)
{
    std::vector<vec3> location = extent(path);
    float y_dist = fabsf(location[1].coord[1] - location[2].coord[1]);
    float x_dist = fabsf(location[1].coord[0] - location[2].coord[0]);
    float max_dist = std::max(y_dist, x_dist);
    return vec3(0, 0,  -mult * (max_dist / 2.0) / 
    (tan(FOV / 2.0 * M_PI / 180.)));
}

void load_and_add(std::string path, Triangle *t, int num_tris, 
std::vector<Shape*> &scenery, vec3 pos, vec3 scaler, color_info info)
{
    tri_ptr(path, t, info);
    add(t, scenery, num_tris, pos, scaler);
}

std::pair<vec3, vec3> scene_extent(const std::vector<Shape*> &scenery)
{
    std::pair<vec3, vec3> result;
    
    vec3 min_coord = vec3(INFINITY, INFINITY, INFINITY);
    vec3 max_coord = vec3(-INFINITY, -INFINITY, -INFINITY);
    for (unsigned int i = 0; i < scenery.size(); i++)
    {
        std::pair<vec3, vec3> obj_bounds = (scenery[i])->bounds();
        for (int j = 0; j < 3; j++)
        {
            if (obj_bounds.first.coord[j] < min_coord.coord[j])
            {
                min_coord.coord[j] = obj_bounds.first.coord[j];
            }
            if (obj_bounds.second.coord[j] > max_coord.coord[j])
            {
                max_coord.coord[j] = obj_bounds.second.coord[j];
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (min_coord.coord[i] == max_coord.coord[i])
        {
            min_coord.coord[i] -= BOX_BIAS;
            max_coord.coord[i] += BOX_BIAS;
        }
    }
    result.first = min_coord;
    result.second = max_coord;
    return result;
}
