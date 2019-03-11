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

float* Sphere::textureCoords(Point hit){ // Return uv-coords, given (x,y,z)-hit-point coordinates

    Point diff = hit - position;
    float theta = acos(diff.z/r);
    float phi = atan2(diff.y, diff.x);
    phi = phi >= 0 ? phi : phi + 2*M_PI;
    //float u = 0.5f + phi/2*M_PI;
    //float v = (M_PI - theta)/M_PI;


    double u = 0.5 + phi/(2*M_PI);
    double v = 1.0 - theta/M_PI;

    return new float[2]{(float)u, (float)v};
}

bool Sphere::hasTexture(){
    return texturePresent;
}

Image Sphere::getTexture(){
    return texture;
}
double Sphere::getRotationAngle(){
    return rotationAngle;
}
Point Sphere::getPosition(){
    return position;
}
Vector Sphere::getRotationAxis(){
    return rotationAxis;
}

Sphere::Sphere(Point const &pos, double radius, double rotAngle, Vector rotAxis)
:
    position(pos),
    r(radius),
    texturePresent(false),
    rotationAngle(rotAngle),
    rotationAxis(rotAxis)
{}

Sphere::Sphere(Point const &pos, double radius, Image tex, double rotAngle, Vector rotAxis)
:
    position(pos),
    r(radius),
    texture(tex),
    texturePresent(true),
    rotationAngle(rotAngle),
    rotationAxis(rotAxis)
{}
