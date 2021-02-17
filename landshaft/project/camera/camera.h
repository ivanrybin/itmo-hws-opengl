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
    const float YAW = -130.0f;
    const float PITCH = -10.0f;
    const float SPEED = 50.0f;
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

    class camera {
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;
        float yaw;      // рыскание (LY)
        float pitch;    // тангаж (LX)
        float move_speed;
        float move_sens;
        float fov;

        camera(glm::vec3 position = glm::vec3(110.0f, 50.0f, 130.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = YAW,
               float pitch = PITCH)
                : front(glm::vec3(-0.6f, 0.5f, -0.75f))
                , move_speed(SPEED)
                , move_sens(SENSITIVITY)
                , fov(FOV)
        {
            this->position = position;
            this->world_up = up;
            this->yaw = yaw;
            this->pitch = pitch;
            update_camera_vecs();
        }

        camera(float posX, float posY, float posZ,
               float upX, float upY, float upZ,
               float yaw, float pitch)
               : front(glm::vec3(0.0f, 0.0f, -1.0f))
               , move_speed(SPEED)
               , move_sens(SENSITIVITY)
               , fov(FOV)
       {
            position = glm::vec3(posX, posY, posZ);
            world_up = glm::vec3(upX, upY, upZ);
            yaw = yaw;
            pitch = pitch;
            update_camera_vecs();
        }

        // Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt-матрицы
        glm::mat4 get_view_matrix() const {
            return glm::lookAt(position, position + front, up);
        }

        void process_keyboard(camera_move direction, float delta_time) {
            float velocity = move_speed * delta_time;

            if (direction == FORWARD) {
                position += front * velocity;
            }
            if (direction == BACKWARD) {
                position -= front * velocity;
            }
            if (direction == LEFT) {
                position -= right * velocity;
            }
            if (direction == RIGHT) {
                position += right * velocity;
            }
        }

        void process_mouse(float xoffset, float yoffset, GLboolean constrainPitch = true) {
            xoffset *= move_sens;
            yoffset *= move_sens;

            yaw += xoffset;
            pitch += yoffset;

            // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
            if (constrainPitch) {
                if (pitch > 89.0f) {
                    pitch = 89.0f;
                } else if (pitch < -89.0f) {
                    pitch = -89.0f;
                }
            }

            // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные значения углов Эйлера
            update_camera_vecs();
        }

        void invert_pitch() {
            pitch = -pitch;
        }

        // Обрабатывает входные данные, полученные от события колеса прокрутки мыши. Интересуют только входные данные на вертикальную ось колесика
        void process_mouse_scroll(float yoffset) {
            if (fov >= 1.0f && fov <= 45.0f) {
                fov -= yoffset;
            } else if (fov <= 1.0f) {
                fov = 1.0f;
            } else if (fov >= 45.0f) {
                fov = 45.0f;
            }
        }

        // Вычисляет вектор-прямо по (обновленным) углам Эйлера камеры
        void update_camera_vecs() {
            // Вычисляем новый вектор-прямо
            glm::vec3 front_tmp;
            front_tmp.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
            front_tmp.y = glm::sin(glm::radians(pitch));
            front_tmp.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
            front = glm::normalize(front_tmp);
            right = glm::normalize(glm::cross(front_tmp, world_up));
            up = glm::normalize(glm::cross(right, front_tmp));
        }
    };
}
