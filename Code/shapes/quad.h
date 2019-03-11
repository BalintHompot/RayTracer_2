#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"
#include "../image.h"

class Quad: public Object
{
    public:
        Quad(Point const &p1, Point const &p2,Point const &p3,Point const &p4);

        virtual Hit intersect(Ray const &ray);
        virtual float *textureCoords(Point hit);
        virtual double getRotationAngle();
        virtual Point getPosition();
        virtual Vector getRotationAxis();

        Point const point1;
        Point const point2;
        Point const point3;
        Point const point4;

};

#endif
