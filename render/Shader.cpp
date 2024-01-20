#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace gl33core;

Shader::Shader()
{
    _program = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(_program);
}

Shader& Shader::source(const char *src, gl::GLenum type)
{

    GLuint target = glCreateShader(type);
    glShaderSource(target, 1, &src, nullptr);
    glCompileShader(target);

    GLboolean status;
    glGetShaderiv(target, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Error compiling shader" << type << std::endl;
        std::cerr << src << std::endl;
        GLint logLength;
        glGetShaderiv(target, GL_INFO_LOG_LENGTH, &logLength);
        char *log = new char[logLength];
        glGetShaderInfoLog(target, logLength, nullptr, log);
        std::cerr << log << std::endl;
        delete[] log;
    }
    else
    {
        glAttachShader(_program, target);
        glDeleteShader(target);
    }

    return *this;
}

Shader& Shader::file(const char* fileName, gl::GLenum type)
{
    std::ifstream in(fileName);
    std::stringstream ss;
    while(in)
    {
        std::string sourceLine;
        std::getline(in, sourceLine);
        ss << sourceLine << "\n";
    }
    return source(ss.str().c_str(), type);
}

Shader& Shader::link()
{
    glLinkProgram(_program);
    GLboolean status;

    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Error linking shader" << std::endl;
        GLint logLength;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
        char *log = new char[logLength];
        glGetProgramInfoLog(_program, logLength, nullptr, log);
        std::cerr << log << std::endl;
        delete[] log;
    }
    return *this;
}

Shader& Shader::use()
{
    glUseProgram(_program);
    return *this;
}

GLint Shader::attrLoc(const char* locName)
{
    return glGetAttribLocation(_program, locName);
}

GLint Shader::unfLoc(const char *unfName)
{
    return glGetUniformLocation(_program, unfName);
}

