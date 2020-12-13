#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../shaders/opengl_shader.h"

struct water_fb {

    unsigned REFLECTION_WIDTH = 1280;
    unsigned REFLECTION_HEIGHT = 720;

    unsigned REFRACTION_WIDTH = 1280;
    unsigned REFRACTION_HEIGHT = 720;

    unsigned reflectionFrameBuffer;
    unsigned reflectionTexture;
    unsigned reflectionDepthBuffer;

    unsigned refractionFrameBuffer;
    unsigned refractionTexture;
    unsigned refractionDepthTexture;

    water_fb() {
        initReflectionFrameBuffer();
        initRefractionFrameBuffer();
    }

    void bindReflectionTexture() {
        glActiveTexture(GL_TEXTURE0 + reflectionTexture);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    }

    void bindRefractionTexture() {
        glActiveTexture(GL_TEXTURE0 + refractionTexture);
        glBindTexture(GL_TEXTURE_2D, refractionTexture);

//        glActiveTexture(GL_TEXTURE0 + refractionDepthTexture);
//        glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
    }

    void bindReflectionFrameBuffer() {
        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    }

    void bindRefractionFrameBuffer() {
        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
    }

    void unbindCurrentFrameBuffer(unsigned screen_width, unsigned screen_height) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screen_width, screen_height);
    }

    void initReflectionFrameBuffer() {
        reflectionFrameBuffer = createFrameBuffer();
        reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        unbindCurrentFrameBuffer(1280, 720);
    }

    void initRefractionFrameBuffer() {
        refractionFrameBuffer = createFrameBuffer();
        refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        unbindCurrentFrameBuffer(1280, 720);
    }

    void bindFrameBuffer(unsigned frameBuffer, unsigned width, unsigned height) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glViewport(0, 0, width, height);
    }

    unsigned createFrameBuffer() {
        unsigned fbo = 0;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        return fbo;
    }

    unsigned createTextureAttachment(unsigned width, unsigned height) {
        unsigned buf_text = 0;
        glGenTextures(1, &buf_text);
        glBindTexture(GL_TEXTURE_2D, buf_text);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buf_text, 0);
        return buf_text;
    }

    unsigned createDepthTextureAttachment(unsigned width, unsigned height) {
        unsigned depth_text = 0;
        glGenTextures(1, &depth_text);
        glBindTexture(GL_TEXTURE_2D, depth_text);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_text, 0);
        return depth_text;
    }

    unsigned createDepthBufferAttachment(unsigned width, unsigned height) {
        unsigned depth_buf = 0;
        glGenRenderbuffers(1, &depth_buf);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_buf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf);
        return depth_buf;
    }
};
