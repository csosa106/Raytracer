#ifndef VEC3_H
#define VEC3_H
#include <iostream>
#include <cmath>

class vec3
{
    public:
        float coord[3];

        vec3()
        {
            coord[0] = 0;
            coord[1] = 0;
            coord[2] = 0;
        }
        vec3(float x, float y, float z)
        {
            coord[0] = x;
            coord[1] = y;
            coord[2] = z;
        }
        float norm() const
        {
           return pow(coord[0] * coord[0] + coord[1] * coord[1] + 
           coord[2] * coord[2], 0.5);
        }
        vec3 normalize() const
        {
            float n = this->norm();
            return vec3(coord[0] / n, coord[1] / n, coord[2] / n);
        }
        vec3 crossProduct(const vec3 right) const
        {
            float x_comp = this->coord[1] * right.coord[2] - 
            this->coord[2] * right.coord[1];
            float y_comp = this->coord[2] * right.coord[0] - 
            this->coord[0] * right.coord[2];
            float z_comp = this->coord[0] * right.coord[1] - 
            this->coord[1] * right.coord[0];
            return vec3(x_comp, y_comp, z_comp);
        }
        vec3 operator-() const
        {
            return vec3(-coord[0], -coord[1], -coord[2]);
        }
        vec3& operator+=(const vec3 &right)
        {
            coord[0] += right.coord[0];
            coord[1] += right.coord[1];
            coord[2] += right.coord[2];
            return *this;
        }
        vec3& operator-=(const vec3 &right)
        {
            *this += -right;
            return *this;
        }
        vec3 reflect(vec3 n) const;
        vec3 refract(vec3 n, float ior1, float ior2) const;
        vec3 comp_mul(const vec3 right) const
        {
            vec3 result;
            for (int i = 0; i < 3; i++)
            {
                result.coord[i] = this->coord[i] * right.coord[i];
            }
            return result;
        }


        vec3 clamp() const
        {
            vec3 result = *this;
            for (int i = 0; i < 3; i++)
            {
                if (result.coord[i] > 1)
                {
                    result.coord[i] = 1;
                }
                else if (result.coord[i] < 0)
                {
                    result.coord[i] = 0;
                }
                
            }
            return result;
        }
};

vec3 operator+(vec3 left, vec3 right);
vec3 operator-(vec3 left, vec3 right);
float operator*(vec3 left, vec3 right);
vec3 operator*(float scalar, vec3 right);
vec3 operator/(vec3 left, float scalar);
bool operator==(vec3 left, vec3 right);
float fresnel(vec3 n, vec3 direction, float ior1, float ior2);
std::ostream& operator<<(std::ostream& os, const vec3 v);
#endif
