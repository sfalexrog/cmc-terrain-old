#include "Context.h"
#include <glbinding/Binding.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>

#include <iostream>

using namespace gl33core;

void debugProc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::cout << source << ": " << type << ": " << severity << ": " << message << std::endl;
}

Context::Context(SDL_Window *w) : _w(w)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    _ctx = SDL_GL_CreateContext(_w);
    if (!_ctx)
    {
        std::cerr << "Could not create context; SDL error: " << SDL_GetError();
    }

    glbinding::Binding::initialize();
    gl33ext::glDebugMessageCallback(debugProc, nullptr);
    glEnable(GL_DEPTH_TEST);
}

Context::~Context()
{
    SDL_GL_DeleteContext(_ctx);
}

void Context::swap()
{
    SDL_GL_SwapWindow(_w);
}

