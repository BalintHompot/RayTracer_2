#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"
#include "../image.h"

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius, double rotAngle, Vector rotAxis);
        Sphere(Point const &pos, double radius, Image tex, double rotAngle, Vector rotAxis);


        virtual Hit intersect(Ray const &ray);

        virtual float* textureCoords(Point hit);

        virtual bool hasTexture();

        virtual Image getTexture();

        Point const position;
        double const r;
        Image texture;
        bool texturePresent;

        Vector rotationAxis;
        double rotationAngle;

        virtual double getRotationAngle();
        virtual Point getPosition();
        virtual Vector getRotationAxis();
};

#endif
