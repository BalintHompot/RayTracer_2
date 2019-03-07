#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"
#include "vertex.h"
#include "mesh.h"

#include <vector>
#include <iostream>

// Forward declerations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    std::vector<Mesh> meshes;
    Point eye;
    int raysPerPixel;

    public:

        // trace a ray into the scene and return the color
        Color trace(Ray const &ray, unsigned rec_depth);

        // render the scene to the given image
        void render(Image &img);


        void addObject(ObjectPtr obj);
        void addMesh(Mesh mesh);
        void addLight(Light const &light);
        void setEye(Triple const &position);
        void setRaysPerPixel(int rpp);

        unsigned getNumObject();
        unsigned getNumLights();
};

#endif
