cmake_minimum_required(VERSION 3.0)
project(terrain)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(deps/glbinding)

add_library(Glm INTERFACE)
target_include_directories(Glm INTERFACE deps/glm)

find_library(_SDL2_LIB SDL2)
find_library(_SDL2MAIN_LIB SDL2main)

find_file(_SDL2_INCLUDE SDL2/SDL.h)

add_library(Sdl2 INTERFACE)
target_link_libraries(Sdl2 INTERFACE ${_SDL2_LIB} ${_SDL2MAIN_LIB})

get_filename_component(_SDL2_INCLUDE_DIR ${_SDL2_INCLUDE} DIRECTORY)
target_include_directories(Sdl2 INTERFACE ${_SDL2_INCLUDE_DIR})

add_library(imgui STATIC
        deps/imgui/imgui.cpp
        deps/imgui/imgui_draw.cpp
        )

target_include_directories(imgui
        PUBLIC
        deps/imgui
        )

add_executable(terrain
        main.cpp
        render/Camera.cpp
        render/Context.cpp
        render/map.cpp
        render/Scene.cpp
        render/Shader.cpp

        render/Texture.cpp
        render/Framebuffer.cpp

        render/WaterController.cpp

        ui/imgui_impl_sdl_gl3.cpp
        )

target_include_directories(terrain
        PRIVATE
        .
        )

target_link_libraries(terrain PRIVATE Sdl2 glbinding Glm imgui)

