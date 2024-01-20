#ifndef TERRAIN_CAMERA_H
#define TERRAIN_CAMERA_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>


class Camera {
public:
    float fov;
    float near;
    float far;

    // Рарзмер кадрового буфера, в который происходит рендер
    glm::vec2 targetSize;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 direction;

    Camera() : fov{glm::pi<float>() / 2.0f}, near{1.0f}, far{100.0f},
               targetSize{glm::vec2{1.0f, 1.0f}},
               position{glm::vec3{0.0f, 0.0f, 0.0f}},
               rotation{glm::vec3{0.0f, 0.0f, 0.0f}},
               direction{glm::vec3{1.0f, 0.0f, 0.0f}}
    {}

    // Получение матрицы вида (позиционирует и направляет камеру)
    glm::mat4 getView();
    // Получение матрицы проекции (матрица для перспективного преобразования)
    glm::mat4 getProj();
};


#endif //TERRAIN_CAMERA_H
