#pragma once

#include <utility>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "../shaders/opengl_shader.h"
#include "../textures/texture.h"

namespace my_model {

    struct landschaft {
        struct vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texture;
        };

        landschaft(const std::string& hmap_file, const std::string& text_file, int step = 1);

        void draw(shader_t& shader, unsigned int texture_id) const;

        glm::vec3 get_normal(size_t z, size_t x, const std::vector<std::vector<glm::vec3>>& grid, size_t width_hmap, size_t height_hmap);
        glm::vec3 get_normal(const glm::vec3& v1, const glm::vec3& v2);
        glm::vec3 normalize_normal(glm::vec3& n1, glm::vec3& n2, glm::vec3& n3, glm::vec3& n4);

        unsigned int VBO, VAO, EBO;
        unsigned int step;

        std::vector<unsigned int>   indices;
        std::vector<vertex>         vertices;
    };

}