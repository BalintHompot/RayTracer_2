#include "quad.h"

#include <cmath>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{


    Triple centerTop = point1;
    double r = 20;
    double b = 2*(ray.O-centerTop).dot(ray.D);
    Point d = ray.O - centerTop;
    double c = d.dot(d)-pow(r,2);

    double descriminant = sqrt(pow(b,2)-4*c);

    if (descriminant < 0){
        return Hit::NO_HIT();
    }

    double t;

    double t1 = (-b + sqrt(pow(b,2)-4*c))/2;
    double t2 = (-b - sqrt(pow(b,2)-4*c))/2;

    if (t1 >= 0)
        t = t1;
    if (t2 >= 0 && t2 < t)
        t = t2;



    // Compute Normal: norm(Point_of_intersection-Center)
    Vector N = (ray.at(t)-centerTop).normalized(); // ray.at(t) == point of intersection


    return Hit(t,N);
}

float *Quad::textureCoords(float x, float y, float z){
    return NULL ;
}

bool Quad::hasTexture(){
    return texture != nullptr;
}

Image Quad::getTexture(){
    return *texture;
}

Quad::Quad(Point const &p1, Point const &p2,Point const &p3,Point const &p4)
:
    point1(point1),
    point2(point2),
    point3(point3),
    point4(point4),
    texture(nullptr)
{}
