# Raytracer C++ framework for Introduction to Computer Graphics

## Modifications for the Raytracer Assignment 2

### Optical laws

#### Producing shadows
In general, when looping over all light sources, a check has been implemented testing whethe another object lies in between a light source and the rays hit-point on an objects surface. If no object lies between the light source and the hit-point, the color calculation proceeds as usual for the considered light source. However, if an object gets detected in between the light source and the hit-point, this light source will not be considered for adding up the color components (specular and diffuse) at the hit-point considered.
For each hit-point on an object's surface, however, at least the ambient light component and the spcular recursive refelction color components will contribute to the hit-point's illumination.
For determining whether another object lies in between a light source and a hit-point, first the vector in the direction from the hit point to the light source gets calculated. Also, a point slightly above the hit point gets calculated to avoid that one objects appears to intersect with itself. This location and the computed direction are then used to construct a ray from the point close to the hit-point pointing to the light source. Then, the program iterates over all objects to test whether the newly constructed ray intersects with at least one of the objects. If it does, this indicates that there lies something on the path from the hit point to the light-source, such that this light-source will not contribute to the illumination of the hit-point considered.

#### Multiple light sources
This part was already implemented in the previous version. Since the scene can contain multiple light sources, we only had to iterate over the light sources in the trace function when calculating the shading (instead of fixing the calculation to the first light source).

#### Specular (recursive) reflection
For implementing the recursion of the trace function, the general procedure is that, first, the standard color computation of the trace-function in scene.cpp gets executed as usual. At the end, however, a new ray gets constructed which gets then passed to the trace function (recursively) again. The return value of the recursive call gets then multiplied by the current material's specular refelction component and the product is going to be added to the overall color evaluated at the currently considered hit-point. This color gets then -as usual- returned by the trace-function. For calculating the new ray to be passed to the recursive call, the origin of the current ray is considered being a light-source, irrespective of whether that is true or whether the ray is a reflected one originating from another refelcting object as well. Therefore, the normalized L-vector, giving the direction from the hit point to the light source is equal to the negative direction of the original (current) ray. Given this vector L, R can be calculated as usual. To avoid that a ray in the next recursive call (in the shadow-calculation) would intersect with itself, the origin of the new ray gets calculated to be located at the current hit point plus a small portion going into the direction of the vector R, which is the vector giving the refelction of the current ray. Then, both this newly calculated location (origin) and the vector R are used to construct a new ray for the next recursive call being a refelction of the current ray.

For setting a maximal recursive depth, another parameter for the trace-function was introduced keeping track of the current recursive depth. Whenever starting a recursive call, the current recursive depth gets decremented from the the current value by one before it gets passed to the next recursive call. If the variable is equal to 0, the base case is reached and the recursion returns Color(0,0,0), since no futher specular components are to be added. To not have to adapt how other parts of the code call the trace function, a default value was assigned to the recursive-depth-indicator, being 2. 

### Anti-aliasing
Anti-aliasing is implemented inside the scene.render function. Originally it was looping over the pixels and shot a ray for each. We implemented an inner loop that shoots a number of rays from each pixel, with the starting position evenly distributed for the rays among the pixel, and average out the results of these rays. The number of rays is taken from the scene json file and stored in the scene object as raysPerPixel.

### Texturing
#### Reading in textures
For reading in textures, it was implemented that when reading in material specifications from a json file, first a check gets performed testing whether a token 'texture' is povided in the material specification of an object or not. If this is the case, and hence a texture (file-)name is provided, one of two overloaded constructors for material gets invoked which receives an Image object (containing the texture image read in from the location: [path + file_name], where path = "../Scenes/" and file_name is the string element specified in the texture token in the scene file. Furthermore, this constructor sets a boolean 'hasTexture' in material to true. No color will be specified for materials having a texture assigned. 
If, however, no texture token is provided in the material specification in a scene file, then a material gets read in as usual. 

#### Retrieving color value from texture for a certain position on an object in the scene
For applying textures to an oject, the trace-function in scene.cpp file gets utilized. First, it checks whether an object has a texture assigned or not by evaluating whether material.hasTexture is true or false. Again, this boolen will be true iff a texture has been assigned to an object when constructing the object. 
If no texture has been assigned to an object, the trace function will continue working as usual. However, if a texture has been provided for an object, then the uv-coordinates for accessing the color value in the texture map for the given object are going to be computed given the hit point, where a ray intersects the respective object for which the material color is to be calculated. The uv-coordinates are calculated according to a mixture of formulas from the slides and the book, since both in isolation didn't work. Following the formulas, u = 0.5 + atan2(y,x)/2pi and v = 1 - acos(z/r)/pi, one has to calculate (x, y, z) = (hit_x, hit_y, hit_z) - (center_object_x, center_object_y, center_object_z) in a piece-wise calculation first. Also, one has to make sure that atan2(y, x) > 0. For that, one needs to add 2pi to the term if it is negative. These calculations apply to the sphere shape, which is the only shape currently for which the calculation of uv coordinates is implemented non-trivially. 
Then, the texture-color gets calculated at the given uv-coordinates given the texture map of the object. This color gets assigned to the material color value. From that point on, the tracing function works as usual again. 

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
