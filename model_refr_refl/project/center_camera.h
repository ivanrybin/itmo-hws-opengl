#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace camera {

    enum camera_move {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Параметры камеры по умолчанию
    const float YAW = 0.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.3f;
    const float ZOOM = 0.3f;

    class camera {
    public:
        glm::vec4 position;
        glm::vec3 up;
        float yaw;      // рыскание (Y)
        float pitch;    // тангаж (X)
        float zoom;

        camera(glm::vec4 position = glm::vec4(0.0f, 0.0f, 5.0f, 1.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
                : position(position)
                , up(up)
                , yaw(YAW)
                , pitch(PITCH)
                , zoom(ZOOM)
        {}

        // Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt-матрицы
        glm::mat4 get_view_matrix() const {
            return glm::lookAt(glm::vec3(position), glm::vec3(0.0f, 0.0f, 0.0f), up);
        }

        void process_mouse(float xoffset, float yoffset) {
            xoffset *= SENSITIVITY;
            yoffset *= SENSITIVITY;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f) {
                pitch = 89.0f;
            } else if (pitch < -89.0f) {
                pitch = -89.0f;
            }
            update_camera_vecs();
        }

        // Обрабатывает входные данные, полученные от события колеса прокрутки мыши. Интересуют только входные данные на вертикальную ось колесика
        void process_mouse_scroll(float yoffset) {
            zoom = (zoom < 0.01f) ? 0.01f : (zoom + yoffset * 0.01f);
//
//            std::cout << zoom << std::endl;
//            if (zoom >= 30.0f && zoom <= 150.0f) {
//                zoom -= yoffset * 0.05;
//            } else if (zoom <= 30.0f) {
//                zoom = 30.0f;
//            } else if (zoom >= 150.0f) {
//                zoom = 150.0f;
//            }
        }
    private:
        void update_camera_vecs() {
            position = glm::vec4(0, 0, 5.0f, 1);
            glm::mat4 rotation_y =  glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0, 1, 0));
            glm::mat4 rotation_x = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(rotation_y * glm::vec4(1, 0, 0, 1)));
            position = rotation_x * (rotation_y * position);
        }
    };
}
