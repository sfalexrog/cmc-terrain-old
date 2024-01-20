#include <iostream>
#include <SDL.h>
#include <glbinding/Binding.h>
#include <glbinding/gl33core/gl.h>

#include <glm/glm.hpp>

#include "render/Context.h"
#include "render/Shader.h"
#include "render/Scene.h"
#include "render/Camera.h"
#include "render/map.h"


#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include "ui/imgui_impl_sdl_gl3.h"

#include "render/Texture.h"
#include "render/Framebuffer.h"
#include "render/WaterController.h"


using namespace gl33core;
using namespace glm;

int main(int argc, char** argv)
{
    srand(time(nullptr));
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *w = SDL_CreateWindow("terrain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 800, SDL_WINDOW_OPENGL);

    Context ctx(w);

#ifdef NO_SHADOWS
    Shader mtShader;
    mtShader.file("shaders/MultitexShader.vert", GL_VERTEX_SHADER)
            .file("shaders/MultitexShader.frag", GL_FRAGMENT_SHADER)
            .link();
#else
    Shader mtShader;
    mtShader.file("shaders/FullLightRender.vert", GL_VERTEX_SHADER)
            .file("shaders/FullLightRender.frag", GL_FRAGMENT_SHADER)
            .link();
#endif


    Shader normShader;
    normShader.file("shaders/NormalsShader.vert", GL_VERTEX_SHADER)
            .file("shaders/NormalsShader.frag", GL_FRAGMENT_SHADER)
            .link();

    Shader nvisShader;
    nvisShader
            .file("shaders/NormalsDisplay.vert", GL_VERTEX_SHADER)
            .file("shaders/NormalsDisplay.geom", GL_GEOMETRY_SHADER)
            .file("shaders/NormalsDisplay.frag", GL_FRAGMENT_SHADER)
                    .link();

    Scene scene;
    Camera camera;

    camera.targetSize = vec2{1280, 800};
    camera.near = 1.0f;
    camera.far = 10000.0f;

    camera.position = vec3{0.0f, 0.0f, 100.0f};

    Camera sunCamera;

    Mesh terrain = createHeightmapMesh(512, 512, 128, 0.0f, 100.0f);

    Texture grass, snow, rocks, sand;
    grass.load("textures/grass3.jpg");
    snow.load("textures/snow.jpg");
    rocks.load("textures/rocks.jpg");
    sand.load("textures/sand.jpg");

    terrain.textures.push_back(std::make_pair(&grass, "grass"));
    terrain.textures.push_back(std::make_pair(&snow, "snow"));
    terrain.textures.push_back(std::make_pair(&rocks, "rocks"));
    terrain.textures.push_back(std::make_pair(&sand, "sand"));

    // Загружаем меш ландшафта в видеопамять и добавляем в сцену
    auto terrainId = scene.addToScene(terrain);
    // Копии для зацикленности ландшафта
    scene.addToScene(terrainId, translate(mat4{1.0f}, vec3{-512.0f, 0.0f, 0.0f}));
    scene.addToScene(terrainId, translate(mat4{1.0f}, vec3{0.0f, -512.0f, 0.0f}));
    scene.addToScene(terrainId, translate(mat4{1.0f}, vec3{-512.0f, -512.0f, 0.0f}));

    // Флаг выхода из основного цикла программы
    bool done = false;

    //Нажата и двигается ли мышь
    bool mouseDragging = false;

    //Перемешения вперед и вбок
    int movementFwd = 0;
    int movementSide = 0;

    //Инициализирование библиотеке ImGui для вывода графических окошек
    ImGui_ImplSdlGL3_Init(w);

    //Флаги отображения нормалей и карты глубины
    bool displayNormals = false;
    bool displayNormalsColor = false;
    bool displayDepthMap = false;

    WaterController wc;

    wc.waterHeight = 10.0f;
    wc.waterSize = vec2{1024.0f, 1024.0f};
    wc.waterShift = vec2{-512.0f, -512.0f};

    auto frameStart = SDL_GetTicks();

    while(!done)
    {
        int deltaX = 0, deltaY = 0;
        int deltaWheel = 0;
        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&ev);

            switch(ev.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouseDragging = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouseDragging = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (mouseDragging)
                    {
                        deltaX += ev.motion.xrel;
                        deltaY += ev.motion.yrel;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            movementFwd = 1;
                            break;
                        case SDL_SCANCODE_S:
                            movementFwd = -1;
                            break;
                        case SDL_SCANCODE_A:
                            movementSide = -1;
                            break;
                        case SDL_SCANCODE_D:
                            movementSide = 1;
                            break;
                        case SDL_SCANCODE_1:
                            displayNormals = false;
                            displayNormalsColor = false;
                            break;
                        case SDL_SCANCODE_2:
                            displayNormalsColor = !displayNormalsColor;
                            break;
                        case SDL_SCANCODE_3:
                            displayNormals = !displayNormals;
                            break;
                        case SDL_SCANCODE_Z:
                            displayDepthMap = !displayDepthMap;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (ev.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_S:
                            movementFwd = 0;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_D:
                            movementSide = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    deltaWheel += ev.wheel.y;
                    break;
                default:
                    break;
            }
        }

        // Начало нового кадра для ImGui
        ImGui_ImplSdlGL3_NewFrame(w);

        // Если курсор мыши вне окон ImGui то примерняем изменения из событий
        if(!ImGui::IsAnyWindowHovered())
        {
            camera.rotation.z += deltaX * 0.001f;
            camera.rotation.x += deltaY * 0.001f;

            camera.direction.x=cos(camera.rotation.x)*cos(camera.rotation.z);
            camera.direction.y=cos(camera.rotation.x)*sin(camera.rotation.z);
            camera.direction.z=sin(camera.rotation.x);
            camera.direction = normalize(camera.direction);

            vec3 sideDirection = cross(camera.direction, vec3(0.0f, 0.0f, 1.0f));

            camera.position += camera.direction * float(movementFwd) + sideDirection * float(movementSide);

            camera.fov -= radians(deltaWheel * 5.0f);
            camera.fov = clamp(camera.fov, radians(5.0f), radians(120.0f));
        }

        // Зацикливание
        if (camera.position.x > 256.0f)
        {
            camera.position.x -= 512.0f;
        }
        if (camera.position.x < -256.0f)
        {
            camera.position.x += 512.0f;
        }
        if (camera.position.y > 256.0f)
        {
            camera.position.y -= 512.0f;
        }
        if (camera.position.y < -256.0f)
        {
            camera.position.y += 512.0f;
        }

        camera.position.z = clamp(camera.position.z, 0.0f, 100.0f);


        ImGui::Begin("Debug window");
            auto frameTime = SDL_GetTicks() - frameStart;
            frameStart = SDL_GetTicks();
            ImGui::Text("Frame time: %d ms (%f fps)", frameTime, 1000.0f / frameTime);
            ImGui::Checkbox("Display normals", &displayNormals);
            ImGui::Checkbox("Display normal colors", &displayNormalsColor);
        ImGui::End();

        // Начинаем рендер
        // Очищаем буфер кадра и буфер глубины
        glClearColor(0, 0, 0, 255);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (displayNormals || displayNormalsColor)
        {
            if (displayNormals)
            {
                scene.render(nvisShader, camera, false, false);
            }
            if (displayNormalsColor)
            {
                scene.render(normShader, camera, false, false);
            }
        }
        else
        {
            // Рендерим сцену в воду
            wc.renderInternal(scene, mtShader, camera);
            scene.render(mtShader, camera, true, true, displayDepthMap);
            wc.renderWater(scene, camera);
        }

        ImGui::Render();

        // Отображаем все что нарисовали (меняем местами буфер отображения и буфер рисования)
        ctx.swap();
    }
    SDL_DestroyWindow(w);

    return 0;
}