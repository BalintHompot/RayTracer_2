#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"
#include "../image.h"

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius);
        Sphere(Point const &pos, double radius, Image tex);

        virtual Hit intersect(Ray const &ray);

        virtual float* textureCoords(float x, float y, float z);

        virtual bool hasTexture();

        virtual Image getTexture();

        Point const position;
        double const r;
        Image *texture;
};

#endif
