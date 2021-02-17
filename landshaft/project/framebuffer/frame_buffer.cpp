#include "frame_buffer.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace my_model {

    void frame_buffer::checkFBO() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "frame_buffer isn't ok" << std::endl;
        }
    }

    void frame_buffer::init() {
        // cubemap + 6 textures
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, text_size, text_size, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }

        // fbo
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        // Create uniform depth buffer for z-test purposes
        glGenRenderbuffers(1, &renderID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, text_size, text_size);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // attach FBO and -LX texture
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboID);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureID, 0);

        checkFBO();
    }

    void frame_buffer::draw_to_side(glm::mat4& view, glm::mat4& proj, int side, glm::vec3 camera_pos) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int) side, textureID, 0);
        checkFBO();

        glClearColor(0, 0, 0, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::vec3 up = glm::vec3(0.0f);
        glm::vec3 center = glm::vec3(0.0f);

        switch (GL_TEXTURE_CUBE_MAP_POSITIVE_X + side) {
            case GL_TEXTURE_CUBE_MAP_POSITIVE_X: {
                center = glm::vec3(1, 0, 0);
                up = glm::vec3(0, -1, 0);
                break;
            } case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: {
                center = glm::vec3(-1, 0, 0);
                up = glm::vec3(0, -1, 0);
                break;
            } case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: {
                center = glm::vec3(0, 1, 0);
                up = glm::vec3(0, 0, -1);
                break;
            } case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: {
                center = glm::vec3(0, -1, 0);
                up = glm::vec3(0, 0, -1);
                break;
            } case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: {
                center = glm::vec3(0, 0, 1);
                up = glm::vec3(0, -1, 0);
                break;
            } case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {
                center = glm::vec3(0, 0, -1);
                up = glm::vec3(0, -1, 0);
                break;
            } default:
                break;
        }
        view = glm::lookAt(camera_pos, center, up);
        proj = glm::perspective<float>(90, 1, 1, 1000);
    }

    void frame_buffer::off() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void frame_buffer::use() {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glViewport(0, 0, text_size, text_size);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
