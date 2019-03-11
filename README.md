# Raytracer C++ framework for Introduction to Computer Graphics

## Modifications for the Raytracer Assignment 2

### Optical laws

#### Shadows

#### Multiple light sources
This part was already implemented in the previous version. Since the scene can contain multiple light sources, we only had iterate over the light sources in the trace function when calculating the shading (instead of fixing the calculation to the first light source).

#### Reflections

### Anti-aliasing
Anti-aliasing is implemented inside the scene.render function. Originally it was looping over the pixels and shot a ray for each. We implemented an inner loop that shoots a number of rays from each pixel, with the starting position evenly distributed for the rays among the pixel, and average out the results of these rays. The number of rays is taken from the scene json file and stored in the scene object as raysPerPixel.

### Texturing
#### Adding texture

#### Rotation
The rotation is implemented using the Rodrigues rotation formula:

v_rotated = v.operator*(cos(rotationAngle)) + (axis.cross(v)).operator*(sin(rotationAngle)) + (axis.operator*(axis.dot(v))).operator*(1-cos(rotationAngle));

Where v is the original point, rotationAngle is the angle with which we rotate and axis is a unit vector starting from the origin, which defines the axis of rotation. 

The rotation happens inside the trace function, and only started when there is texture. What we do essentially is that when a hot point is found, and we have texture, before retrieveng the coordinates on the texture that correspond to the hit, we rotate the hit point, and retrieve the texture for the rotated point (so we basically rotate the texture on the object). Since the Rodrigues formula rotates about the origin, first we need to move the center of the sphere to the origin, perform the rotation and then move it back.

The parameters are taken from the scene json file for each object separately (if it is not specified, they are set to 0), and accessed in the trace function through the object. For this purpose, the object object had to be extended with a getPosition (for accessing the center of the sphere), getRotationAxis and getRotationAngle functions. Note that the rotation axis is stored in the object as it is in the json, and only gets normalized when tracing, and also the rotation angle is stored in degrees, as it is in the json, and only gets converted when tracing.


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
