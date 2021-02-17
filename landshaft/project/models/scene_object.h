#pragma once

#include <functional>
#include "model.h"
#include "../camera/camera.h"
#include "../gui/gui_handler.h"

struct scene_object {
    scene_object() {}

    void draw(const gui_handler& gui);
    void draw(const glm::mat4& view, const glm::mat4& proj, const gui_handler& gui);
    void translate(float x, float y, float z);
    void scale(float s);

    my_model::model* model  = nullptr;
    camera::camera*  cam    = nullptr;
    shader_t*        shader = nullptr;

    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 P = glm::mat4(1.0f);

    float scale_ = 1.0f;

    float trans_X = 0.0f;
    float trans_Y = 0.0f;
    float trans_Z = 0.0f;

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    bool is_camera_pos  = false;
    bool need_update    = true;
    bool is_light       = false;

    std::vector<std::function<void()>> tasks{};

    int    counter  = 0;
    float lastTime  = 0.0f;
    float startTime = 0.0f;
    float      sign = 1.0f;

    bool    need_rotate = false;
    float         angle = 0.0f;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
};