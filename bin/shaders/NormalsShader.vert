#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec3 norm;
layout(location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vnorm;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    vnorm = normalize(norm);
}
