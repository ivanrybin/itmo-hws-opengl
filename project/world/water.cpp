#include "water.h"
#include "../shaders/opengl_shader.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace my_model {

    water::water() {
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec3 color(0.0f, 0.8f, 1.0f);

        glm::vec3 left_up   (-1.0f,  0.0f,   1.0f);  //
        glm::vec3 left_down (-1.0f,  0.0f,  -1.0f);  //
        glm::vec3 right_up  ( 1.0f,  0.0f,  1.0f);   //
        glm::vec3 right_down( 1.0f,  0.0f, -1.0f);   //

        glm::vec2 left_up_text      (0.0f, 1.0f);
        glm::vec2 left_down_text    (0.0f, 0.0f);
        glm::vec2 right_up_text     (1.0f, 1.0f);
        glm::vec2 right_down_text   (1.0f, 0.0f);


        vertices.push_back({left_up, normal, left_up_text});
        vertices.push_back({left_down, normal, left_down_text});
        vertices.push_back({right_up, normal, right_up_text});
        vertices.push_back({right_down, normal, right_down_text});

        indices = {
                0, 1, 2,
                1, 2, 3
        };
    }
    void water::init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Координаты вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) 0);

        // Нормали вершин
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) sizeof(glm::vec3));

        // Текстурные координаты вершин
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (2 * sizeof(glm::vec3)));

        glBindVertexArray(0);
    }

    void water::draw(shader_t& shader) {
        // Отрисовываем figure
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

}
