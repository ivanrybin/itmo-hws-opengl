#pragma once

#include <utility>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "../shaders/opengl_shader.h"
#include "../textures/texture.h"

namespace my_model {

    struct vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };

    struct mesh {
        mesh(std::vector<unsigned int> indices,
             std::vector<vertex>  vertices,
             std::vector<texture> textures) : indices(std::move(indices))
                                            , vertices(std::move(vertices))
                                            , textures(std::move(textures)) { init(); }

        void draw(shader_t& shader) const;
        void init();

        unsigned int VBO, VAO, EBO;

        std::vector<unsigned int>   indices;
        std::vector<vertex>         vertices;
        std::vector<texture>        textures;
    };

}
