#include "objloader.h"

// Pro C++ Tip: here you can specify other includes you may need
// such as <iostream>
#include <limits>       /* Infinity */
#include <stdlib.h>     /* abs */
#include <cmath>        /* abs */
#include "triple.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// ===================================================================
// -- Constructors and destructor ------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------

OBJLoader::OBJLoader(string const &filename)
:
    d_hasTexCoords(false)
{
    parseFile(filename);
    create_data(); // Creating data batch here prevents having to compute it everytime again mesh is going to be requested
}

// ===================================================================
// -- Member functions -----------------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------

vector<Vertex> OBJLoader::vertex_data() const
{
    return data;    // copy elision
}

unsigned OBJLoader::numTriangles() const
{
    return d_vertices.size() / 3U;
}

bool OBJLoader::hasTexCoords() const
{
    return d_hasTexCoords;
}

void OBJLoader::unitize()
{
    // TODO: implement this yourself!

    // This is a very handy function for importing models
    // which you may reuse in other projects.
    // You may have noticed you can use arbitrary sizes for your
    // models. You may find that modelers do not always use the
    // same size for models. Therefore it might be preferable to
    // scale the object to fit inside the unit cube so you can easily
    // set the right scale of your model in OpenGL. Aditionally,
    // the model does not have to be centered around the origin
    // (0, 0, 0) which may cause troubles when translating
    // This function should fix that!

    // A common approach looks like this:

    // Determine min / max and offset in each dimension
    // Determine by which factor to scale (largest difference
    //  in min / max in a dimension (Important! Scale uniformaly in
    //  all dimensions!)
    // Loop over all coordinate data and scale the coordinates
    //  and apply the translate/scaling

    double min_x = std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double min_z = std::numeric_limits<double>::infinity();
    Vertex centroid = {0,0,0,0,0,0,0,0};
    double scaling_factor = 0;

    for (unsigned i = 0; i < data.size(); i++)
    {
        // Determine smallest offset in each dimension to make equal to 0/positve
        if (data[i].x < min_x) min_x = data[i].x;
        if (data[i].y < min_y) min_y = data[i].y;
        if (data[i].z < min_z) min_z = data[i].z;

        // Determine center of mesh
        centroid.x += data[i].x;
        centroid.y += data[i].y;
        centroid.z += data[i].z;

        // Determine scaling
        if (abs(data[i].x) > scaling_factor) scaling_factor = abs(data[i].x);
        if (abs(data[i].y) > scaling_factor) scaling_factor = abs(data[i].y);
        if (abs(data[i].z) > scaling_factor) scaling_factor = abs(data[i].z);
    }

    // Calculate average over all data points and scale result down
    centroid.x /= (data.size() * scaling_factor);
    centroid.y /= (data.size() * scaling_factor);
    centroid.z /= (data.size() * scaling_factor);

    min_x /= scaling_factor;
    min_y /= scaling_factor;
    min_z /= scaling_factor;

    //cout << "Min in 3 dimensions: " << " : " << min_x << ", " << min_y << ", " << min_z << "\n";
    //cout << "Centroid: " << " : " << centroid.x << ", " << centroid.y << ", " << centroid.z << "\n";
    //cout << "Scaling: " << scaling_factor << "\n";

    for (unsigned i = 0; i < data.size(); i++)
    {
        // Apply scaling
        data[i].x /= scaling_factor;
        data[i].y /= scaling_factor;
        data[i].z /= scaling_factor;

        // Apply translation (center around origin)
        data[i].x -= centroid.x;
        data[i].y -= centroid.y;
        data[i].z -= centroid.z;
    }

    /*for (unsigned i = 0; i < data.size(); i++)
    {
        cout << i << ": Scaled and transformed: "  << data[i].x << ", " << data[i].y << ", " << data[i].z << "\n";
    }*/



    // Put item fully back into scene (in case of negative coords)
    if (min_x < 0) for (unsigned i = 0; i < data.size(); i++) data[i].x += abs(min_x);

    if (min_y < 0) for (unsigned i = 0; i < data.size(); i++) data[i].y += abs(min_y);

    if (min_z < 0) for (unsigned i = 0; i < data.size(); i++) data[i].z += abs(min_z);


    /*for (unsigned i = 0; i < data.size(); i++)
    {
        cout << i << ": Set bove 0: "  << data[i].x << ", " << data[i].y << ", " << data[i].z << "\n";
    }*/

    for (unsigned i = 0; i < data.size(); i++)
    {
        //cout << "Unscaled: " << i << " : " << data[i].x << ", " << data[i].y << ", " << data[i].z << "\n";
        // Apply scaling (make unit-mesh a bit larger)
        data[i].x *= 60;
        data[i].y *= 60;
        data[i].z *= 60;
        //cout << "Scaled: " << i << " : " << data[i].x << ", " << data[i].y << ", " << data[i].z << "\n";
    }

    cerr << "Unitized.\n";
}

// --- Private -------------------------------------------------------

std::vector<Vertex> OBJLoader::create_data()
{
    // For all vertices in the model, interleave the data
    for (Vertex_idx const &vertex : d_vertices)
    {
        // Add coordinate data
        Vertex vert;

        vec3 const coord = d_coordinates.at(vertex.d_coord);
        vert.x = coord.x;
        vert.y = coord.y;
        vert.z = coord.z;

        // Add normal data
        vec3 const norm = d_normals.at(vertex.d_norm);
        vert.nx = norm.x;
        vert.ny = norm.y;
        vert.nz = norm.z;

        // Add texture data (if available)
        if (d_hasTexCoords)
        {
            vec2 const tex = d_texCoords.at(vertex.d_tex);
            vert.u = tex.u;      // u coordinate
            vert.v = tex.v;      // v coordinate
        } else {
            vert.u = 0;
            vert.v = 0;
        }
        data.push_back(vert);
    }

    return data;    // copy elision
}

void OBJLoader::parseFile(string const &filename)
{
    ifstream file(filename);
    if (file)
    {
        string line;
        while(getline(file, line))
            parseLine(line);

    } else {
        cerr << "Could not open: " << filename << " for reading!\n";
    }
}

void OBJLoader::parseLine(string const &line)
{
    if (line[0] == '#')
        return;                     // ignore comments

    StringList tokens = split(line, ' ', false);

    if (tokens[0] == "v")
        parseVertex(tokens);
    else if (tokens[0] == "vn")
        parseNormal(tokens);
    else if (tokens[0] == "vt")
        parseTexCoord(tokens);
    else if (tokens[0] == "f")
        parseFace(tokens);

    // Other data is also ignored
}

void OBJLoader::parseVertex(StringList const &tokens)
{
    float x, y, z;
    x = stof(tokens.at(1));         // 0 is the "v" token
    y = stof(tokens.at(2));
    z = stof(tokens.at(3));
    d_coordinates.push_back(vec3{x, y, z});
}

void OBJLoader::parseNormal(StringList const &tokens)
{
    float x, y, z;
    x = stof(tokens.at(1));         // 0 is the "vn" token
    y = stof(tokens.at(2));
    z = stof(tokens.at(3));
    d_normals.push_back(vec3{x, y, z});
}

void OBJLoader::parseTexCoord(StringList const &tokens)
{
    d_hasTexCoords = true;          // Texture data will be read

    float u, v;
    u = stof(tokens.at(1));         // 0 is the "vt" token
    v = stof(tokens.at(2));
    d_texCoords.push_back(vec2{u, v});
}

void OBJLoader::parseFace(StringList const &tokens)
{
    // skip the first token ("f")
    for (size_t idx = 1; idx < tokens.size(); ++idx)
    {
        // format is:
        // <vertex idx + 1>/<texture idx +1>/<normal idx + 1>
        // Wavefront .obj files start counting from 1 (yuck)

        StringList elements = split(tokens.at(idx), '/');
        Vertex_idx vertex {}; // initialize to zeros on all fields

        vertex.d_coord = stoul(elements.at(0)) - 1U;

        if (d_hasTexCoords)
            vertex.d_tex = stoul(elements.at(1)) - 1U;
        else
            vertex.d_tex = 0U;       // ignored

        vertex.d_norm = stoul(elements.at(2)) - 1U;

        d_vertices.push_back(vertex);
    }
}

OBJLoader::StringList OBJLoader::split(string const &line,
                            char splitChar,
                            bool keepEmpty)
{
    StringList tokens;
    istringstream iss(line);
    string token;
    while (getline(iss, token, splitChar))
        if (token.size() > 0 || (token.size() == 0 && keepEmpty))
            tokens.push_back(token);

    return tokens;
}
