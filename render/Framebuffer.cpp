#include "Framebuffer.h"

using namespace glm;
using namespace gl;

Framebuffer::Framebuffer(ivec2 size): _size(size)
{
    glGenFramebuffers(1, &_framebufferId);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_framebufferId);
};

void Framebuffer::attachDepthTexture(Texture *target)
{
    GLint prevBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target->getId(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
}

void Framebuffer::attachColorTexture(Texture *target)
{
    GLint prevBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target->getId(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
}

void Framebuffer::attachRenderbuffer()
{
    GLint prevBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    GLuint rb;
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
}

void Framebuffer::activate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
}
