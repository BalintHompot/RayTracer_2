#include "plane.h"

#include <cmath>

Hit Plane::intersect(Ray const &ray)
{
    // Point + Normal define plane; plane should be orthogonal to Normal

    // N (is normalized) is given by definition of plane

    double numerator = (point.normalized().operator-(ray.O.normalized())).dot(N);
    double denumerator = ray.D.normalized().dot(N);

    if (denumerator == 0) // If true, plane and ray are parallel & not intersecting
        return Hit::NO_HIT();

    double t = (numerator/denumerator);
    //t = (t > 0 ? t : -t);
    return Hit(t, N);
}

Plane::Plane(Point const &pt, Point const &n)
:
    point(pt),
    N(n)
{}
