#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

#include <glbinding/gl33core/gl.h>

class Shader {
private:
    gl::GLuint _program;
public:
    Shader();
    ~Shader();

    // Загрузка в компилятор драйвера шейдера из строки в которой содержаться исходники
    Shader& source(const char* src, gl::GLenum type);
    // Загрузка шейдера из файла исходного кода шейдера
    Shader& file(const char* fileName, gl::GLenum type);
    // Объединение шейдеров в программу
    Shader& link();
    // Использование шейдера - назначаем шейдер текущим
    Shader& use();

    // Получение индекса вершинного массива
    gl::GLint attrLoc(const char* attrName);
    // Получение индекса юниформы
    gl::GLint unfLoc(const char* unfName);

};


#endif //TERRAIN_SHADER_H
