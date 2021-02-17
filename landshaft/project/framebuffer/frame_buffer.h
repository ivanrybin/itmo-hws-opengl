#pragma once

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>

namespace my_model {
    struct frame_buffer {
        frame_buffer(unsigned text_size = 1024) : text_size(text_size) {}

        void init();
        void checkFBO();
        void use();
        void off();
        void draw_to_side(glm::mat4& view, glm::mat4& proj, int side, glm::vec3 camera_pos);

        GLuint textureID = 0;
        GLuint fboID = 0;
        GLuint renderID = 0;
        unsigned text_size;
    };

}