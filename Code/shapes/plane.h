#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"
#include "../image.h"

class Plane: public Object
{
    public:
        Plane(Point const &pt, Point const &n);

        virtual Hit intersect(Ray const &ray);
        virtual float *textureCoords(Point hit);
        virtual double getRotationAngle();
        virtual Point getPosition();
        virtual Vector getRotationAxis();

        /* YOUR DATA MEMBERS HERE*/
        Point const point;
        Vector const N;
};

#endif
