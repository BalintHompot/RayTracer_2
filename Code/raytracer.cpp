#include "raytracer.h"
#include "objloader.h"
#include "mesh.h"
#include "vertex.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"


// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];

        Vector rotationAxis = Vector(0,0,0);
        double rotationAngle = 0;
        // Read texture if present
/*         try{
            rotationAxis = Vector(node["rotation"]);
            rotationAngle = node["angle"];
        }catch (const std::exception &exc) {
            rotationAxis = Vector(0,0,0);
            rotationAngle = 0;
            cerr << "no angle or axis found for rotation: " << exc.what() << "\n";
        } */
        if (node.find("rotation") != node.end()){
            rotationAxis = Vector(node["rotation"]);
        }
        if (node.find("angle") != node.end()){
            rotationAngle = node["angle"];
        }

        if (node.find("RelativeImagePath") != node.end()){
            string path = node["RelativeImagePath"];
            std::cout << "Texture stated to be at location: " << path << '\n';
            obj = ObjectPtr(new Sphere(pos, radius, Image(path), rotationAngle, rotationAxis));
        } else {
            // No texture present
            obj = ObjectPtr(new Sphere(pos, radius, rotationAngle, rotationAxis));
        }



    } else if (node["type"] == "triangle")
    {
        //cerr << "Object type: " << node["type"] << ".\n";
        Point pos1(node["position1"]);
        Point pos2(node["position2"]);
        Point pos3(node["position3"]);
        obj = ObjectPtr(new Triangle(pos1, pos2, pos3));
    } else if (node["type"] == "quad")
    {
        Point pos1(node["point1"]);
        Point pos2(node["point2"]);
        Point pos3(node["point3"]);
        Point pos4(node["point4"]);
        obj = ObjectPtr(new Triangle(pos1, pos2, pos3));
        // Parse material and add object to the scene
        obj->material = parseMaterialNode(node["material"]);
        scene.addObject(obj);
        obj = ObjectPtr(new Triangle(pos1, pos3, pos4));
        // Parse material and add object to the scene
        obj->material = parseMaterialNode(node["material"]);
        scene.addObject(obj);
        return true;
    } else if (node["type"] == "plane")
    {
        Point pos(node["position"]);
        Point norm(node["normal"]);
        obj = ObjectPtr(new Plane(pos, norm.normalized()));
    } else
    {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }
// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    Color color(node["color"]);
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    return Material(color, ka, kd, ks, n);
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // Reading in the output image size if specified in json-file
    try {
        cout << "Parsed size? " << jsonscene["Size"] <<"\n";
        int sizes[2] = {jsonscene["Size"][0],jsonscene["Size"][1]};
        width = sizes[0];
        height = sizes[1];
    } catch (const std::exception &exc) {
        cerr << "Something went wrong: " << exc.what() << "\n";
    }

    try{
        int raysPerPixel = jsonscene["SuperSamplingFactor"];
        scene.setRaysPerPixel(raysPerPixel);
    } catch (const std::exception &exc) {
        scene.setRaysPerPixel(4);
    }

    // Reading in meshes if declared in json-file
    try {
        for (auto const &meshNode : jsonscene["Meshes"])
        {
            cout << "Parsed mesh: " << meshNode["name"] << "\n"; // handy: get type of json object in string format: meshNode["name"].type_name() or jSonObj.type_name()
            std::string str = meshNode["name"];
            OBJLoader loader = OBJLoader(str);
            loader.unitize();

            //scene.addMesh(Mesh(loader.vertex_data())); // Time didn't allow implementing this aprroach entirely yet. So, the following is gonna be performed instead:

            for (unsigned i = 0; i < loader.vertex_data().size(); i++)
            {
                for (unsigned ii = 0; ii < loader.vertex_data().size(); ii++)
                {
                    for (unsigned iii = 0; iii < loader.vertex_data().size(); iii++)
                    {
                        if (i != ii && i != iii && ii != iii){
                            Point a = Point(loader.vertex_data()[i].x, loader.vertex_data()[i].y, loader.vertex_data()[i].z);
                            Point b = Point(loader.vertex_data()[ii].x, loader.vertex_data()[ii].y, loader.vertex_data()[ii].z);
                            Point c = Point(loader.vertex_data()[iii].x, loader.vertex_data()[iii].y, loader.vertex_data()[iii].z);
                            ObjectPtr obj = nullptr;
                            obj = ObjectPtr(new Triangle(a, b, c));
                            if (obj)
                            {
                                // Parse material (average color from all 3 points) and add object to the scene
                                //Color avg = Color(loader.vertex_data()[i].r, loader.vertex_data()[i].g, loader.vertex_data()[i].b).operator+(...); // Color not given in appropriate format
                                //obj->material;
                                scene.addObject(obj);
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::exception &exc) {
        cerr << "Something went wrong: " << exc.what() << "\n";
    }

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    cout << "Parsed lights.\n";

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    Image img(width, height);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
