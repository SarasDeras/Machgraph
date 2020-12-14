#ifndef MASHGRAPH_CAMERA_H
#define MASHGRAPH_CAMERA_H

#define GLEW_STATIC
#include "libs/include/glew.h"
#include "libs/include/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Варианты движения камерой
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Параметры камеры по умолчанию
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    //Атрибуты камеры
    glm::vec3 Pos;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    //углы Эйлера
    float Yaw;
    float Pitch;
    //Настройки камеры
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up =
            glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(-1.0f, 0.0f, 0.0f),
            float yaw = YAW, float pitch = PITCH): MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Pos = position;
        WorldUp = up;
        Front = front;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Pos, Pos + Front, Up);
    }
    void key_callback(Camera_Movement direction, float deltaTime) {
        float v = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Pos += Front * v;
        if (direction == BACKWARD)
            Pos -= Front * v;
        if (direction == LEFT)
            Pos -= Right * v;
        if (direction == RIGHT)
            Pos += Right * v;
    }
    void mouse_callback(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные значения углов Эйлера
        updateCameraVectors();
    }

    void scroll_callback(double xoffset, double yoffset) {
        float max_fov = 45.0f;
        float min_fov = 1.0f;
        Zoom -= yoffset;
        if (Zoom > max_fov){
            Zoom = max_fov;
        }
        if (Zoom <= min_fov){
            Zoom = min_fov;
        }
    }

    void updateCameraVectors() {
        // Вычисляем новые вектора
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

};


#endif //MASHGRAPH_CAMERA_H
