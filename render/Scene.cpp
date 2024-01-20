#include "Scene.h"

#include <glbinding/gl33core/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <imgui.h>
#include <deps/glm/glm/gtc/matrix_transform.hpp>

using namespace gl33core;

using namespace glm;

const ivec2 shadowSize = ivec2{1024, 1024};

struct SkyMesh
{
    struct Vertex
    {
        vec3 pos;
    };
    std::vector<Vertex> v;
    std::vector<uint32_t> indices;
};

SkyMesh createSun()
{
    SkyMesh sun{};
    sun.v.push_back(SkyMesh::Vertex{vec3{0.0f, 0.0f, 0.0f}});
    const int NUM_SIDES = 6;
    sun.v.push_back(SkyMesh::Vertex{vec3{1.0f, 0.0f, 0.0f}});

    for(int i = 1; i < NUM_SIDES; ++i)
    {
        vec3 v1 = vec3(cos(2 * i * pi<float>() / NUM_SIDES), sin(2 * i * pi<float>() / NUM_SIDES), 0.0f);
        sun.v.push_back(SkyMesh::Vertex{v1});
        sun.indices.push_back(0);
        sun.indices.push_back(sun.v.size() - 2);
        sun.indices.push_back(sun.v.size() - 1);
    }
    sun.indices.push_back(0);
    sun.indices.push_back(sun.v.size() - 1);
    sun.indices.push_back(1);

    return sun;
}

Scene::Scene() : _meshes{}, _shadowBuffer{shadowSize}
{
    glGenVertexArrays(1, &_vao);

    glBindTexture(GL_TEXTURE_2D, _shadowTexture.getId());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowSize.x, shadowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    _shadowBuffer.attachDepthTexture(&_shadowTexture);

    _sunCamera.position = vec3{0.0f, 0.0f, 150.0f};
    _sunCamera.direction = vec3{-0.5f, 0.0f, -0.3f};
    _sunCamera.near = -500.0f;
    _sunCamera.far = 1000.0f;
    _sunCamera.fov = 0.0f;
    _sunCamera.targetSize = shadowSize;

    _shadowShader.file("shaders/Depth.vert", GL_VERTEX_SHADER)
            .file("shaders/Depth.frag", GL_FRAGMENT_SHADER)
            .link();

    _skyTexture.loadCubemap("textures/sky");

    SkyMesh mesh;
    mesh.v.push_back(SkyMesh::Vertex{vec3{-1, 1, 1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{-1, -1, 1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{1, -1, 1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{1, 1, 1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{-1, 1, -1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{-1, -1, -1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{1, -1, -1}});
    mesh.v.push_back(SkyMesh::Vertex{vec3{1, 1, -1}});

    mesh.indices = {
            0, 1, 2,
            2, 3, 0,
            2, 3, 6,
            3, 7, 6,
            6, 7, 4,
            6, 4, 5,
            4, 5, 0,
            5, 1, 0,
            0, 4, 7,
            0, 7, 3,
            1, 5, 6,
            1, 6, 2
    };

    glGenBuffers(1, &_skyBox.vbo);
    glGenBuffers(1, &_skyBox.ibo);

    glBindBuffer(GL_ARRAY_BUFFER, _skyBox.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.v.size() * sizeof(SkyMesh::Vertex), mesh.v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _skyBox.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    _skyShader.file("shaders/Skybox.vert", GL_VERTEX_SHADER)
            .file("shaders/Skybox.frag", GL_FRAGMENT_SHADER)
            .link();

    _sunShader.file("shaders/SunShader.vert", GL_VERTEX_SHADER)
            .file("shaders/SunShader.frag", GL_FRAGMENT_SHADER)
            .link();

    auto sunMesh = createSun();

    glGenBuffers(1, &_sun.vbo);
    glGenBuffers(1, &_sun.ibo);
    glBindBuffer(GL_ARRAY_BUFFER, _sun.vbo);
    glBufferData(GL_ARRAY_BUFFER, sunMesh.v.size() * sizeof(SkyMesh::Vertex), sunMesh.v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sun.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sunMesh.indices.size() * sizeof(uint32_t), sunMesh.indices.data(), GL_STATIC_DRAW);
    _sun.idxSize = sunMesh.indices.size();
}

Scene::~Scene()
{
    for(const auto& mesh : _meshes)
    {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ibo);
    }
    glDeleteVertexArrays(1, &_vao);
}

int Scene::addToScene(const Mesh& mesh, glm::mat4 transform)
{
    Scene::GpuMesh gpuMesh;
    gpuMesh.transform = transform;
    gpuMesh.idxSize = mesh.indices.size();

    glGenBuffers(1, &gpuMesh.vbo);
    glGenBuffers(1, &gpuMesh.ibo);

    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.v.size() * sizeof(Mesh::Vertex), mesh.v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
    gpuMesh.textures = mesh.textures;
    gpuMesh.textures.push_back(std::make_pair(&_shadowTexture, "shadowMap"));

    _meshes.push_back(gpuMesh);
    return _meshes.size() - 1;
}

int Scene::addToScene(int gpuMeshId, glm::mat4 transform)
{
    _meshes.push_back(_meshes[gpuMeshId]);
    _meshes[_meshes.size() - 1].transform = transform;
    return _meshes.size() - 1;
}

void Scene::_renderScene(Shader &s, Camera &c)
{
    glBindVertexArray(_vao);
    s.use();

    if (_clipPlane.x != 0.0f || _clipPlane.y != 0.0f || _clipPlane.z != 0.0f)
    {
        glEnable(GL_CLIP_DISTANCE0);
    }

    GLint posLoc = s.attrLoc("pos");
    GLint colLoc = s.attrLoc("col");
    GLint normLoc = s.attrLoc("norm");
    GLint uvLoc = s.attrLoc("uv");

    if (posLoc > -1)
    {
        glEnableVertexAttribArray(posLoc);
    }
    if (colLoc > -1)
    {
        glEnableVertexAttribArray(colLoc);
    }
    if (normLoc > -1)
    {
        glEnableVertexAttribArray(normLoc);
    }
    if (uvLoc > -1)
    {
        glEnableVertexAttribArray(uvLoc);
    }

    GLint modelLoc = s.unfLoc("model");
    GLint viewLoc = s.unfLoc("view");
    GLint projectionLoc = s.unfLoc("projection");
    GLint depthLoc = s.unfLoc("DepthMVP");
    GLint depthBiasLoc = s.unfLoc("DepthMVPBiased");
    GLint lightDirLoc = s.unfLoc("lightDirInverse");
    GLint planeLoc = s.unfLoc("clipPlane");

    if (planeLoc > -1)
    {
        glUniform4fv(planeLoc, 1, value_ptr(_clipPlane));
    }

    auto view = c.getView();
    if (viewLoc > -1) {glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));}
    auto proj = c.getProj();
    if (projectionLoc > -1) {glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));}

    if (lightDirLoc > -1)
    {
        glUniform3f(lightDirLoc, -_sunCamera.direction.x,
                    -_sunCamera.direction.y,
                    -_sunCamera.direction.z);
    }

    for(const auto& mesh : _meshes)
    {
        auto DepthMVP = _sunCamera.getProj() * _sunCamera.getView() * mesh.transform;
        if (depthLoc > -1)
        {
            glUniformMatrix4fv(depthLoc, 1, GL_FALSE, value_ptr(DepthMVP));
        }
        if (depthBiasLoc > -1)
        {
            auto bias = mat4{
                    0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0,
                    0.0, 0.0, 0.5, 0.0,
                    0.5, 0.5, 0.5, 1.0
            };
            auto DepthBiasMVP = bias * DepthMVP;
            glUniformMatrix4fv(depthBiasLoc, 1, GL_FALSE, value_ptr(DepthBiasMVP));
        }
        if (modelLoc > -1) {glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(mesh.transform));}

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

        if (posLoc > -1)
        {
            glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, pos));
        }
        if (colLoc > -1)
        {
            glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, col));
        }
        if (normLoc > -1)
        {
            glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, norm));
        }
        if (uvLoc > -1)
        {
            glVertexAttribPointer(uvLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, uv));
        }

        int texIdx = 0;
        for (auto& texData : mesh.textures)
        {
            auto texLoc = s.unfLoc(texData.second.c_str());
            if (texLoc > -1)
            {
                glActiveTexture(GL_TEXTURE0 + texIdx);
                glBindTexture(GL_TEXTURE_2D, texData.first->getId());
                glUniform1i(texLoc, texIdx);

            }
            texIdx += 1;
        }
        glDrawElements(GL_TRIANGLES, mesh.idxSize, GL_UNSIGNED_INT, 0);
    }

    if (_clipPlane.x != 0.0f || _clipPlane.y != 0.0f || _clipPlane.z != 0.0f)
    {
        glDisable(GL_CLIP_DISTANCE0);
    }
}

void Scene::_renderShade()
{
    FramebufferBinding b(_shadowBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 2.0f);

    vec4 oldClipPlane = _clipPlane;
    _clipPlane = vec4{0.0};

    _renderScene(_shadowShader, _sunCamera);

    _clipPlane = oldClipPlane;

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Scene::_renderSky(Camera &c)
{
    glBindVertexArray(_vao);
    _skyShader.use();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindBuffer(GL_ARRAY_BUFFER, _skyBox.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _skyBox.ibo);
    auto posLoc = _skyShader.attrLoc("pos");
    glEnableVertexAttribArray(posLoc);

    auto mvpLoc = _skyShader.unfLoc("MVP");

    Camera skyCamera = c;
    skyCamera.position = vec3{0.0, 0.0, 0.0};
    skyCamera.near = 0.1f;
    skyCamera.far = 1000.0f;

    auto skyMvp = skyCamera.getProj() * skyCamera.getView();

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(skyMvp));

    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(SkyMesh::Vertex), 0);
    glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyTexture.getId());

    auto texLoc = _skyShader.unfLoc("skybox");
    glUniform1i(texLoc, 0);


    _sunShader.use();
    glBindBuffer(GL_ARRAY_BUFFER, _sun.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sun.ibo);

    auto sunColorLoc = _sunShader.unfLoc("sunColor");

    posLoc = _sunShader.attrLoc("pos");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(SkyMesh::Vertex), 0);

    auto sunSzLoc = _sunShader.unfLoc("sunSize");
    auto viewLoc = _sunShader.unfLoc("view");
    auto projLoc = _sunShader.unfLoc("projection");
    auto sunPosLoc = _sunShader.unfLoc("sunPosition");

    auto sunPos = getSunPosition();

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(skyCamera.getView()));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(skyCamera.getProj()));
    glUniform3f(sunPosLoc, sunPos.x, sunPos.y, sunPos.z);
    glUniform1f(sunSzLoc, 2.5f);
    glUniform3f(sunColorLoc, 1.0f, 1.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, _sun.idxSize, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Scene::render(Shader &s, Camera &c, bool withShadows, bool withSkybox, bool showDepthMap)
{
    if (withShadows)
    {
        _renderShade();
    }
    if (withSkybox)
    {
        _renderSky(c);
    }
    _renderScene(s, c);

    if (showDepthMap)
    {
        ImGui::Begin("Depth map");
        ImGui::Image((ImTextureID)_shadowTexture.getId(), ImVec2(512, 512));
        ImGui::End();
    }
}

void Scene::setClipPlane(glm::vec4 plane)
{
    _clipPlane = plane;
}

vec3 Scene::getSunPosition()
{
    return normalize(-_sunCamera.direction) * 50.0f;
}
