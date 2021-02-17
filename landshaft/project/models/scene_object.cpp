#include "scene_object.h"

void scene_object::draw(const gui_handler& gui) {
    draw(V, P, gui);
}

void scene_object::draw(const glm::mat4& view, const glm::mat4& proj, const gui_handler& gui) {
    shader->use();

        if (need_update) {
            M = glm::mat4(1.0f);
            M = glm::translate(M, glm::vec3(X, Y, Z));
            M = glm::translate(M, glm::vec3(trans_X, trans_Y, trans_Z));
            M = glm::scale(M, glm::vec3(scale_));
            if (need_rotate) {
                M = glm::rotate(M, angle, direction);
            }
            need_update = false;
        }

        shader->set_mat4("V", view);
        shader->set_mat4("M", M);
        shader->set_mat4("P", proj);

        if (is_light) {
            shader->set_uniform<float>("ambientStrength", gui.ambient_strength);
            shader->set_uniform<float>("diffuseStrength", gui.diffuse_strength);
            shader->set_vec3("lightColor", glm::vec3(gui.LR, gui.LG, gui.LB));
            shader->set_vec3("lightPos", glm::vec3(gui.LX, gui.LY, gui.LZ));
        }

        for (const auto& task : tasks) {
            task();
        }

        if (is_camera_pos) {
            shader->set_vec3("cameraPos", glm::vec3(cam->position));
        }

        if (model != nullptr) {
            model->draw(*shader);
        }

    shader->off();
}

void scene_object::translate(float x, float y, float z) {
    trans_X += x;
    trans_Y += y;
    trans_Z += z;
    need_update = true;
}

void scene_object::scale(float s) {
    if (scale_ != s) {
        scale_ = s;
        need_update = true;
    }
}
