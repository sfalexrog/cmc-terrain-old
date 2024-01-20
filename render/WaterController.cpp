#include "WaterController.h"
#include <glm/glm.hpp>
#include <glbinding/gl33core/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <SDL.h>

using namespace glm;
using namespace gl;

ivec2 texSize = ivec2{512, 512};

WaterController::WaterController() : _reflectFb(texSize), _refractFb(texSize)
{
    _waterBump.load("textures/normal.png");
    _waterDuDv.load("textures/waterDUDV.png");

    _waterShader.file("shaders/Water.vert", GL_VERTEX_SHADER).file("shaders/Water.frag", GL_FRAGMENT_SHADER).link();

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ibo);

    vec3 quadPos[] = {vec3{0.0f, 0.0f, 0.0f},
                      vec3{1.0f, 0.0f, 0.0f},
                      vec3{0.0f, 1.0f, 0.0f},
                      vec3{1.0f, 1.0f, 0.0f}
    };

    uint32_t quadIdx[] = {
            0, 1, 2,
            1, 3, 2
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos), quadPos, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIdx), quadIdx, GL_STATIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, _reflection.getId());
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texSize.x, texSize.y);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glBindTexture(GL_TEXTURE_2D, _refraction.getId());
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texSize.x, texSize.y);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glBindTexture(GL_TEXTURE_2D, _refractionDepth.getId());
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, texSize.x, texSize.y);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    _reflectFb.attachColorTexture(&_reflection);
    _reflectFb.attachRenderbuffer();

    _refractFb.attachColorTexture(&_refraction);
    //_refractFb.attachDepthTexture(&_refractionDepth);
    _refractFb.attachRenderbuffer();
}

WaterController::~WaterController()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ibo);
}

void WaterController::renderInternal(Scene &scene, Shader &shader, Camera &c)
{
    {
        FramebufferBinding reflect(_reflectFb);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera reflectionCamera(c);
        float distance = c.position.z - waterHeight;
        reflectionCamera.position.z -= 2 * distance;
        reflectionCamera.rotation.x = - c.rotation.x;

        reflectionCamera.direction.x=cos(reflectionCamera.rotation.x)*cos(reflectionCamera.rotation.z);
        reflectionCamera.direction.y=cos(reflectionCamera.rotation.x)*sin(reflectionCamera.rotation.z);
        reflectionCamera.direction.z=sin(reflectionCamera.rotation.x);
        
        scene.setClipPlane(vec4{0.0f, 0.0f, 1.0f, -waterHeight});
        scene.render(shader, reflectionCamera, false);
    }
    {
        FramebufferBinding refract(_refractFb);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera refractionCamera(c);

        scene.setClipPlane(vec4{0.0f, 0.0f, -1.0f, waterHeight});
        scene.render(shader, refractionCamera, false);
    }
    // Отключаем ограничивающую плоскость
    scene.setClipPlane(vec4{0.0f, 0.0f, 0.0f, 0.0f});
#ifdef DEBUG_WATER
    ImGui::Begin("Reflection debug");
        ImGui::Image((ImTextureID)_reflection.getId(), ImVec2(512, 512));
    ImGui::End();

    ImGui::Begin("Refraction debug");
        ImGui::Image((ImTextureID)_refraction.getId(), ImVec2(512, 512));
        ImGui::Image((ImTextureID)_refractionDepth.getId(), ImVec2(512, 512));
    ImGui::End();
#endif //DEBUG_WATER
}

void WaterController::renderWater(Scene &scene, Camera &c)
{
    _waterShader.use();

    mat4 modelMatrix = mat4{1.0f};
    modelMatrix = translate(modelMatrix, vec3{waterShift.x, waterShift.y, waterHeight});
    modelMatrix = scale(modelMatrix, vec3{waterSize.x, waterSize.y, 1.0});

    auto modelPos = _waterShader.unfLoc("model");
    glUniformMatrix4fv(modelPos, 1, GL_FALSE, value_ptr(modelMatrix));
    auto viewPos = _waterShader.unfLoc("view");
    glUniformMatrix4fv(viewPos, 1, GL_FALSE, value_ptr(c.getView()));
    auto projPos = _waterShader.unfLoc("projection");
    glUniformMatrix4fv(projPos, 1, GL_FALSE, value_ptr(c.getProj()));

    auto reflectPos = _waterShader.unfLoc("reflection");
    auto refractPos = _waterShader.unfLoc("refraction");
    auto dudvPos = _waterShader.unfLoc("dudv");
    auto timePos = _waterShader.unfLoc("time");
    auto camPos = _waterShader.unfLoc("camPosition");
    auto normalMapPos = _waterShader.unfLoc("normalMap");
    auto lightPos = _waterShader.unfLoc("lightPosition");

    float timeSec = SDL_GetTicks() / 1000.0f;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _reflection.getId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _refraction.getId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _waterDuDv.getId());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _waterBump.getId());

    glUniform1i(reflectPos, 0);
    glUniform1i(refractPos, 1);
    glUniform1i(dudvPos, 2);
    glUniform1i(normalMapPos, 3);

    glUniform1f(timePos, timeSec);
    glUniform3fv(camPos, 1, value_ptr(c.position));
    glUniform3fv(lightPos, 1, value_ptr(scene.getSunPosition()));

    auto posLoc = _waterShader.attrLoc("pos");
    glEnableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
