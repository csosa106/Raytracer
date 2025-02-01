#include "drawer.hpp"

void draw(const kdtree &t, std::vector<Light> &lights, vec3 eye, vec3 center, 
vec3 up, int rows, int cols, float ior, int depth, float bias, float fov, 
std::string filename, float rgb[HEIGHT][WIDTH][3])
{
    float total = HEIGHT * WIDTH;
    float *mat = lookAtMatrix(eye, center, up);
    
    # pragma omp parallel for
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            std::cout << 100. * float(i * WIDTH + j) / total << 
            "% done" << std::endl;
            vec3 new_point = lookAt(mat, primary(i, j, FOV));
            vec3 new_dir = new_point.normalize();
            vec3 color = caster(eye, new_dir, t, lights, depth);
            for (int k = 0; k < 3; k++)
            {
                rgb[i][j][k] = color.coord[k];
            }
        }
    }
    delete mat;

    std::ofstream file(filename + ".txt");
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                file << rgb[i][j][k] << " ";
            }
            file << std::endl;
        }
        file << std::endl;
    }
    file.close();
}
