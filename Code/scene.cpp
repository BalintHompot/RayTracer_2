#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;


Color Scene::trace(Ray const &ray, unsigned rec_depth = 2) // Max 2 recursive calls (by default)
{
    if (rec_depth <= 0) return Color(0.0,0.0,0.0); // Max recursive depth reached; Base-case: return

    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material material = obj->material;          //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    /*
   if (obj->hasTexture()){
        // Texture found

        // Compute uv-coordinates from hit-point (only non-trivially implemented for sphere currently)

        /// rotation formula (Rodrigues)
        /// we always rotate along the y axis (0,1,0)
        /// transform the hit coordinates to get rotated texture position
        double rotationAngle = 5;
        Vector axis = Vector(0,1,0);
        Vector originalHit = hit;
        hit = hit.operator*(cos(rotationAngle)) + (axis.cross(hit)).operator*(sin(rotationAngle)) + (axis.operator*(axis.dot(hit))).operator*(1-cos(rotationAngle));

        float *uv = obj->textureCoords(hit.x, hit.y, hit.z);
        //std::cout << "uv-coords: " << uv[0] << ", " << uv[1] << "\t"; // print for debugging

        try {
            // Load texture from object (only non-trivially implemented for sphere currently)
            Image img = obj->getTexture();

            // Put texture color into material color
            material.color = img.colorAt(uv[0], uv[1]);

        } catch (const std::exception &exc) {
            cerr << "Something went wrong when aplying texture in scene.cpp: " << exc.what() << "\n";
        }

        hit = originalHit;
    }
    */

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color color = (material.color.operator*(material.ka)); // Ambient part, not if computing specular reflection only

    bool shadow = false;

    for (int i = 0; i < (int)lights.size(); i++){

        shadow = false; // Default: no shadow; include light-source into illumination calculation
        Vector toLight = (lights[i]->position - hit).normalized(); // Normalized vector towards light source (from hit point)
        Point above_hit_position = ray.at(min_hit.t-0.01f); // Position slightly above hit point to prevent points intersection with itself; 0.01f = safety margin

        // Test whether shadow is to be produced
        for (unsigned ii = 0; ii != objects.size(); ++ii)
        {
            Hit objectHit(objects[ii]->intersect(Ray(above_hit_position, toLight)));
            if (objectHit.t > 0) // Intersection with object: shadow --> Don't consider this light source
            {
                shadow = true;
                break;
            }
        }


        if (!shadow){ // Skip this light source if shadow = true; Else perform illumination calculation for light source
            // ALl vectors normalized
            Vector L = (lights[i]->position - hit).normalized(); // Calculate L
            Vector R = N.operator*(2*N.dot(L)).operator-(L);

            // Diffuse
            double id = L.dot(N);
            id = (id < 0 ? 0 : id); // max(0, N*L) a bit more generalized to make L*N always positive
            Color Id1 = material.color.operator*(id);
            Color Id2 = lights[i]->color.operator*(material.kd);
            Color diffuse = Id1.operator*(Id2);


            // Specular
            double i_dot = R.dot(V);
            i_dot = (i_dot < 0 ? 0 : i_dot); // max()
            i_dot = pow(i_dot, material.n);
            Color specular = lights[i]->color.operator*(i_dot*material.ks);

            // Adding terms
            color.operator+=(diffuse.operator+=(specular)); // Add all light-parts up
        }


    }
    // Specular reflection

    Vector L = -ray.D; // Calculate L
    Vector R = N.operator*(2*N.dot(L)).operator-(L);

    Point above_hit_position = hit + R * 0.01; // Move hit point a bit further in refelctance direction

    color += trace(Ray(above_hit_position, R), rec_depth-1) * material.ks;

    return color;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            Color col =  Color(0,0,0);
            ///Multiple rays per pixel for anti-aliasing
            float raysInARow = sqrt(raysPerPixel);
            float step_offset = 1/raysInARow;
            float start_offset = step_offset/2;
            for(float x_offset = start_offset; x_offset<1; x_offset+= step_offset){
                for(float y_offset = start_offset; y_offset<1; y_offset+= step_offset){
                    Point pixel(x + x_offset, h - 1 - y + y_offset, 0);
                    Ray ray(eye, (pixel - eye).normalized());
                    Color newCol = trace(ray);
                    col.operator+=(newCol);

                }
            }
            col.operator/=(raysPerPixel);

            col.clamp();
            img(x, y) = col;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}


void Scene::addMesh(Mesh mesh){
    meshes.push_back(mesh);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

void Scene::setRaysPerPixel(int rpp){
    raysPerPixel = rpp;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
