#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../camera/camera.h"

struct gui_handler {
    gui_handler(GLFWwindow* window, const char *glsl_version);
    ~gui_handler();

    void draw(camera::camera& scene_camera);
    void off();

    int texture_repeats = 10;
    bool is_polygons = false;
    bool is_skybox = true;

    // back color
    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;

    float LR = 1.0f;
    float LG = 1.0f;
    float LB = 1.0f;

    // scene coords
    float LX = 0;
    float LY = 0;
    float LZ = 0;

    float clip_dist = -0.163;

    float   diffuse_strength = 0.5;
    float   ambient_strength = 0.1;
};

GLuint init_square(float* s);

void draw_square(GLuint VAO);
