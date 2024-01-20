#ifndef TERRAIN_FRAMEBUFFER_H
#define TERRAIN_FRAMEBUFFER_H

#include <glm/vec2.hpp>
#include <glbinding/gl33core/gl.h>

#include "Texture.h"

class Framebuffer {
private:
    // Хранит данные для использования кадрового буфера на карте
    //Индекс кадрового буфера
    gl::GLuint _framebufferId;
    glm::ivec2 _size;

public:
    Framebuffer(glm::ivec2 fbSize);
    ~Framebuffer();

    // Добавление текстуры глубины в буфер
    void attachDepthTexture(Texture *target);
    void attachColorTexture(Texture *target);
    // Если не нужна карта глубины
    void attachRenderbuffer();

    glm::ivec2 getSize() {return _size;};
    // Назначает буфер текущим, в него будет рендер
    void activate();
};

// Привязывает и отвязывает кадровый буфер
// При создании запоминает тот буфер, который был; при уничтожении возвращает старый буфер
class FramebufferBinding
{
private:
    gl::GLint _prevBuffer;
    // Размер запомненного буфера
    gl::GLint _vp[4];
public:
    FramebufferBinding(Framebuffer &fb)
    {
        gl::glGetIntegerv(gl::GL_FRAMEBUFFER_BINDING, &_prevBuffer);
        gl::glGetIntegerv(gl::GL_VIEWPORT, _vp);
        fb.activate();
        auto fbSize = fb.getSize();
        gl::glViewport(0, 0, fbSize.x, fbSize.y);
    }

    ~FramebufferBinding()
    {
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, _prevBuffer);
        gl::glViewport(_vp[0], _vp[1], _vp[2], _vp[3]);
    }
};


#endif //TERRAIN_FRAMEBUFFER_H
