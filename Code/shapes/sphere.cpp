#include "sphere.h"
#include "solvers.h"

#include <cmath>
#include <iostream>
#include <math.h>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    // Sphere formula: ||x - position||^2 = r^2
    // Line formula:   x = ray.O + t * ray.D

   
    Vector L = ray.O - position;


    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(L);
    double c = L.dot(L) - r * r;

    

    double t0;
    double t1;
    if (not Solvers::quadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();

    // t0 is closest hit
    if (t0 < 0)  // check if it is not behind the camera
    {
        t0 = t1;    // try t1
        if (t0 < 0) // both behind the camera
            return Hit::NO_HIT();
    }

    // calculate normal
    Point hit = ray.at(t0);

    Vector N = (hit - position).normalized();

    // determine orientation of the normal
    if (N.dot(ray.D) > 0)
        N = -N;

    return Hit(t0, N);
}

float* Sphere::textureCoords(float x, float y, float z){ // Return uv-coords, given (x,y,z)-hit-point coordinates
    float rad;
    if (r == 0){ // Avoid 0-division
        rad = 0.00001f;
    } else {
        rad = r;
    }

    float u = 0.5f + (atan2(y,x))/(2*M_PI);
    float v = 1.0f - acos(((float)z/(float)rad))/(float)M_PI;

    //float *arr = new float[2];
    //arr[0] = u;
    //arr[1] = v;
    //return arr;
    return new float[2]{u, v};
}

bool Sphere::hasTexture(){
    return texture != nullptr;
}

Image Sphere::getTexture(){
    return *texture;
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius),
    texture(nullptr)
{}

Sphere::Sphere(Point const &pos, double radius, Image tex)
:
    position(pos),
    r(radius),
    texture(&tex)
{}
