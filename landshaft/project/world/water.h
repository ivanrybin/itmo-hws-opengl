#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../shaders/opengl_shader.h"

namespace my_model {

    struct water {
        struct vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 tex_coords;
        };

        water();

        void init();
        void draw(shader_t& shader);

        unsigned VBO, VAO, EBO;
        std::vector<vertex> vertices{};
        std::vector<unsigned> indices{};
    };

}