#version 330

in vec3 pos;
out vec3 uv;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(pos, 1.0);
    uv = pos.xzy;
}