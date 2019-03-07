# Raytracer C++ framework for Introduction to Computer Graphics

## Modifications for the Raytracer Assignment 1

### Spheres
For the spheres we had to implement the intersection and the normal calculation. To calculate the intersection we made the formula for the sphere equal to the formula of the ray, reduce the equation to a quadratic formula of the distance, and solve the quadratic. The intersection to be returned is the minimum of the two roots (or the one if there is one, or nothing if there is none).
The Normal is easy from this step, we take the difference of the intersection and the center and normalize it.

### Phong illumination
To calculate the Phong illumination, we followed the formulas on the slides. For each light source, we calculate separately the ambient, the diffuse and the specular components. The material properties are given in the scene, the normal for specular is returned from the shape's intersect method (see above for sphere). The three components are summed for the full return value.

### Triangles
For the triangles we implemented the Möller–Trumbore intersection algorithm, following this site:
https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
This algorithm calculates the intersection point and the norm similarly to a plane, and after that checks if the triangle is in front of the ray (so it should be visible), and if the intersection of the ray and the triangle's plane is within the boundaries defined by the edges of the triangle (in which case we actually should return a hit).
Rendering triangles is functional, but the positions are somehow off, even if we define two vertices to be the same and the triangles only differ in one vertex, the same points do not render to the same position. We could not find out why this happens.
### Planes
Planes are defined by a point and a Normal. The intersection calculation was adapted from https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection .

### Quads
Our implementation of the quad is just taking 4 points, and render 4 trinagles to obtain a tetrahedron-like object (it was not entirely clear what a quad is, but if it is just 4 points in some way, we can simply only render 2 triangles and get a rendered 4 points). Unfortunately, since the triangles render into unexpected positions, the edges of these triangles won't meet, so we won't get a correct quad, although it is just the problem mentioned in the triangles section. 

### Scene and Object loading
Scene-files may now contain both a size-specification for the output image (width*height-format) and one or multiple file names of meshes (e.g. "cube.obj") for meshes which are located in the Scenes-folder. Only the file name has to be provided for a mesh-object and the rest of the relative path to the object will be considered automatically. Also, if no size-specification is given in a scene-file, then the default output-image-size is going to be 400*400 pixels. 

Objects are now read in by the OBJLoader class and the vertices present in such a file are automatically converted to a set of all possible triangles that can be created from the set of vertices. This set of triangles gets then attached to the scene to be rendered as usual triangles. In spite of starting implementing a mesh-object-infrastructure in the code which would treat each mesh as a separate object, time didn't allow finalizing this approach. Since this functionality may be useful for the future, though, it was decided to keep the efforts made for implementing meshes as objects predent in the code (in order to continue on that approach later). It needs to be said that while loading and processing mesh-data works (as was tested by including print-statements which are disabled in the submission), the final rendering of meshes gives unexpected results, where the mesh does not look as expected, but has weird shates. We attribute this to the potetial error when rendering triangles (as indicated above).

Also, unitization for meshes was implemented. First, a mesh gets scaled down to fit into a unit cube and afterwards its center gets translated to the origin of the graph. Since the output image only starts at the origin, and hence to put an object fully into the visual scene, afterwards, the objects gets translated again such that it has only positive coordinates anymore and afterwards, it gets scaled by factor 60 to make it better visible again (after unitization). For simplicity, we decided to put all this functionality into the unitization function. 


## Original Readme

## Compiling the code

We provided a [CMakeLists.txt](CMakeLists.txt) file to be used with `cmake`.
This can be used in the following way (on the command line/shell):
```
# cd into the main directory of the framework
mkdir build   # create a new build directory
cd build      # cd into the directory
cmake ..      # creates a Makefile
# Comipiling the code can then be done with:
make
# or
make -j4      # replacing 4 with the number of cores of your pc
```
**Note!** After adding new `.cpp` files (when adding new shapes)
`cmake ..` needs to be called again or you might get linker errors.

## Running the Raytracer
After compilation you should have the `ray` executable.
This can be used like this:
```
./ray <path to .json file> [output .png file]
# when in the build directory:
./ray ../Scenes/scene01.json
```
Specifying an output is optional and by default an image will be created in
the same directory as the source scene file with the `.json` extension replaced
by `.png`.

## Description of the included files

### Scene files
* `Scenes/*.json`: Scene files are structured in JSON. If you have never
    worked with json, please see [here](https://en.wikipedia.org/wiki/JSON#Data_types,_syntax_and_example)
    or [here](https://www.json.org/).

    Take a look at the provided example scenes for the general structure.
    You are free (and encouraged) to define your own scene files later on.

### The raytracer source files (Code directory)

* `main.cpp`: Contains main(), starting point. Responsible for parsing
    command-line arguments.

* `raytracer.cpp/.h`: Raytracer class. Responsible for reading the scene
    description, starting the raytracer and writing the result to an image file.

* `scene.cpp/.h`: Scene class. Contains code for the actual raytracing.

* `image.cpp/.h`: Image class, includes code for reading from and writing to PNG
    files.

* `light.h`: Light class. Plain Old Data (POD) class. Colored light at a
    position in the scene.

* `ray.h`: Ray class. POD class. Ray from an origin point in a direction.

* `hit.h`: Hit class. POD class. Intersection between an `Ray` and an `Object`.

* `object.h`: virtual `Object` class. Represents an object in the scene.
    All your shapes should derive from this class. See

* `shapes (directory/folder)`: Folder containing all your shapes.

* `sphere.cpp/.h (inside shapes)`: Sphere class, which is a subclass of the
    `Object` class. Represents a sphere in the scene.

* `example.cpp/.h (inside shapes)`: Example shape class. Copy these two files
    and replace/rename **every** instance of `Example` `example.h` or `EXAMPLE`
    with your new shape name.

* `triple.cpp/.h`: Triple class. Represents a 3-dimensional vector which is
    used for colors, points and vectors.
    Includes a number of useful functions and operators, see the comments in
    `triple.h`.
    Classes of `Color`, `Vector`, `Point` are all aliases of `Triple`.

* `objloader.cpp/.h`: Is a similar class to Model used in the OpenGL
    exercises to load .obj model files. It produces a std::vector
    of Vertex structs. See `vertex.h` on how you can retrieve the
    coordinates and other data defined at vertices.

### Supporting source files (Code directory)

* `lode/*`: Code for reading from and writing to PNG files,
    used by the `Image` class.
    lodepng is created by Lode Vandevenne and can be found on
    [github](https://github.com/lvandeve/lodepng).
* `json/*`: Code for parsing JSON documents.
    Created by Niels Lohmann and available under the MIT license on
    [github](https://github.com/nlohmann/json).
    **Recommended:** Especially take a look at their README for more
    info on how to work with json files.
