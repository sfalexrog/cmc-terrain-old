#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec3 norm;
layout(location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 DepthMVPBiased;

// Вектор, противоположный направлению освещения
uniform vec3 lightDirInverse;

out vec3 vcol;
out vec2 vuv;
out float height;
out vec4 shadowCoord;

// Направление освещения относительно камеры
out vec3 lightDirCam;
// Направление взгляда относительно камеры
out vec3 eyeDirCam;
// Позиция нормали вершины относительно камеры
out vec3 normCam;
// Позиция вершины относительно начала координат сцены
out vec3 posWorld;
// Глубина точки
out float pointDepth;

// Коэффициенты уравнения плоскости
uniform vec4 clipPlane;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    vcol = col;
    vuv = uv;
    height = pos.z;
    shadowCoord = DepthMVPBiased * vec4(pos, 1.0);

    lightDirCam = (view * vec4(lightDirInverse, 0.0)).xyz;
    eyeDirCam = vec3(0.0, 0.0, 0.0) - (view * model * vec4(pos, 1.0)).xyz;

    vec4 posWorldFull = (model * vec4(pos, 1.0));
    posWorld = posWorldFull.xyz;
    normCam = (view * model * vec4(norm, 0.0)).xyz;
    pointDepth = gl_Position.z;

    gl_ClipDistance[0] = dot(posWorldFull, clipPlane);

}
