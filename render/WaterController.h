#ifndef TERRAIN_WATERCONTROLLER_H
#define TERRAIN_WATERCONTROLLER_H

#include <glbinding/gl33core/gl.h>

#include "Scene.h"
#include "Texture.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Camera.h"

#include <glm/vec2.hpp>

class WaterController
{
private:
    // БУферы отражения и преломления(видимость дна)
    Framebuffer _reflectFb;
    Framebuffer _refractFb;

    Texture _reflection;
    Texture _refraction;
    Texture _refractionDepth;

    // Текстура смещений волн
    Texture _waterDuDv;
    // Текстура рельефа воды
    Texture _waterBump;

    Shader _waterShader;

    gl::GLuint _vbo;
    gl::GLuint _ibo;

public:
    WaterController();
    ~WaterController();

    float waterHeight;
    glm::vec2 waterSize;
    glm::vec2 waterShift;

    // Рендер всей сцены в текстуру воды
    void renderInternal(Scene &scene, Shader &shader, Camera &c);
    // Рендер воды в текущий кадровый буфер
    void renderWater(Scene &scene, Camera &c);
};



#endif //TERRAIN_WATERCONTROLLER_H
