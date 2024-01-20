#ifndef TERRAIN_MESH_H
#define TERRAIN_MESH_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <string>

class Texture;

struct Mesh
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec3 col;
        glm::vec2 uv;
    };
    std::vector<Vertex> v;
    std::vector<uint32_t> indices;
    std::vector<std::pair<Texture*, std::string>> textures;

    Mesh(): v{}, indices{} {};
};

#endif //TERRAIN_MESH_H
