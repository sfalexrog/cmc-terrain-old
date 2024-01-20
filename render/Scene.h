#ifndef TERRAIN_SCENE_H
#define TERRAIN_SCENE_H

#include <vector>
#include <glbinding/gl33core/gl.h>
#include <glm/mat4x4.hpp>
#include <string>

#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"


class Scene {
private:
    // GpuMesh - структура, содержащая данные о меше, загруженном в видеопамять.
    // Эта структура НЕ содержит данные о вершинах; вместо этого в ней есть
    // номера буферов, в которых содержатся сами координаты и параметры вершин.
    struct GpuMesh
    {
        // Вершинный буфер - хранит параметры вершины (позиция, нормаль и т.д.)
        gl::GLuint vbo;
        // Буфер индексов - хранит индексы вершин, объединяемых в примитивы (треугольники)
        gl::GLuint ibo;
        // Размер буфера индексов (количество элементов в нём)
        size_t idxSize;
        // Матрица преобразования для данного меша
        glm::mat4 transform;
        // Текстуры, которые использует данный меш (с их именами для привязки в шейдере)
        std::vector<std::pair<Texture*, std::string>> textures;
    };
    std::vector<GpuMesh> _meshes;
    // Объект вершинных массивов - содержит всё, что нужно для передачи данных вершин в шейдеры.
    gl::GLuint _vao;
    Camera _sunCamera;
    // Буфер карты глубины (теней)
    Framebuffer _shadowBuffer;
    // Текстура карты глубины (теней)
    Texture _shadowTexture;
    Shader _shadowShader;

    Texture _skyTexture;

    GpuMesh _skyBox;
    Shader _skyShader;

    GpuMesh _sun;
    Shader _sunShader;

    // Рендер объектов загруженных в сцену
    void _renderScene(Shader &s, Camera &c);
    // Составление карты теней
    void _renderShade();
    // Рендер неба
    void _renderSky(Camera &c);

    glm::vec4 _clipPlane;

public:

    Scene();
    ~Scene();

    // Добавление меша с загрузкой в память
    int addToScene(const Mesh& mesh, glm::mat4 transform = glm::mat4{1.0f});
    // Дублирование в сцене уже загруженного меша
    int addToScene(int gpuMeshId, glm::mat4 transform = glm::mat4{1.0f});

    // Установка плоскости среза воды
    void setClipPlane(glm::vec4 plane);
    // Получение позиции солнца
    glm::vec3 getSunPosition();

    //Общий рендер
    void render(Shader &s, Camera &c, bool withShadows = true, bool withSkybox = true, bool showDepthMap = false);
};

#endif //TERRAIN_SCENE_H
