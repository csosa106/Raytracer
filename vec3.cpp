#include "vec3.hpp"

vec3 operator+(vec3 left, vec3 right)
{
    return vec3(left) += right;
}

vec3 operator-(vec3 left, vec3 right)
{
    return vec3(left) += -right;
}

float operator*(vec3 left, vec3 right)
{
    return left.coord[0] * right.coord[0] + left.coord[1] * right.coord[1] + 
    left.coord[2] * right.coord[2];
}

vec3 operator*(float scalar, vec3 right)
{
    return vec3(scalar * right.coord[0], scalar * right.coord[1], 
    scalar * right.coord[2]);
}

vec3 operator/(vec3 left, float scalar)
{
    return (1.0 / scalar) * left;
}

vec3 vec3::reflect(vec3 n) const
{
    return *this - (2.0 * ((*this * n) * n));
}

// https://stackoverflow.com/questions/29758545/how-to-find-
// refraction-vector-from-incoming-vector-and-surface-normal
// return 0 vector in case of total internal reflection
vec3 vec3::refract(vec3 n, float ior1, float ior2) const
{
    float eta = ior1 / ior2;
    float cosi = fabsf(n * *this);
    float sint2 = eta * eta * (1 - cosi * cosi);
    if (sint2 > 1.0)
    {
        return vec3(0, 0, 0);
    }
    else
    {
        float cost = pow(1.0 - sint2, .5);
        return eta * *this + (eta * cosi - cost) * n;
    }
    
}

bool operator==(vec3 left, vec3 right)
{
    bool result = true;
    for (int i = 0; i < 3; i++)
    {
        if (left.coord[i] != right.coord[i])
        {
            return false;
        }
    }
    return result;
}

// Only called when there is no total internal reflection
float fresnel(vec3 n, vec3 direction, float ior1, float ior2)
{
    float reflect; 
    float reflect_parallel;
    float reflect_perpendicular;
    float cosi = fabsf(direction * n);
    float eta = ior1 / ior2;
    float sint2 = eta * eta * (1 - cosi * cosi);
    float cost = pow(1 - sint2, .5);

    reflect_parallel = pow((ior2 * cosi - ior1 * cost) / 
    (ior2 * cosi + ior1 * cost), 2);
    reflect_perpendicular = pow((ior1 * cosi - ior2 * cost) / 
    (ior1 * cosi + ior2 * cost), 2);

    reflect = .5 * (reflect_parallel + reflect_perpendicular);
    return 1 - reflect;
}

std::ostream& operator<<(std::ostream& os, const vec3 v)
{
    os << "(" << v.coord[0] << ", " << v.coord[1] << ", " << v.coord[2] << ")";
    return os;
}
