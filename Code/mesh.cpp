#include "mesh.h"

// ===================================================================
// -- Constructors and destructor ------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------

Mesh::Mesh(std::vector<Vertex> data)
{
    model = data;
}

// ===================================================================
// -- Member functions -----------------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------


std::vector<Vertex> Mesh::get_vertices() const
{
    return model;    // copy elision
}