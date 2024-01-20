#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec3 norm;
layout(location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 DepthMVP;

out vec3 vcol;
out vec2 vuv;
out float height;
out vec4 shadowCoord;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    vcol = col;
    vuv = uv;
    height = pos.z;
    shadowCoord = DepthMVP * vec4(pos, 1.0);
}
