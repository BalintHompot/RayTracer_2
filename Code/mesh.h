#ifndef MESH_H_
#define MESH_H_

#include "vertex.h"
#include <vector>

class Mesh
{
    std::vector<Vertex> model;
    std::vector<Vertex> getModel();


    public:

        /**
         * @brief Mesh
         * @param vector of vertices
         */
        explicit Mesh(std::vector<Vertex> const data);

        /**
         * @brief vertex_data
         * @return interleaved vertex data, see vertex.h
         */
        std::vector<Vertex> get_vertices() const;

};

#endif // MESH_H_
