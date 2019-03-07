#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane: public Object
{
    public:
        Plane(Point const &pt, Point const &n);

        virtual Hit intersect(Ray const &ray);

        /* YOUR DATA MEMBERS HERE*/
        Point const point;
        Vector const N;
};

#endif
