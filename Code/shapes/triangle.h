#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"
#include "../image.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &v0,
                 Point const &v1,
                 Point const &v2);

        virtual Hit intersect(Ray const &ray);
        virtual float* textureCoords(Point hit);
        virtual bool hasTexture();
        virtual Image getTexture();
        virtual double getRotationAngle();
        virtual Point getPosition();
        virtual Vector getRotationAxis();

        Point v0;
        Point v1;
        Point v2;
        Vector N;
        Image *texture;
};

#endif
