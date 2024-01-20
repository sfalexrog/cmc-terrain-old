#version 330

in vec3 vnorm;

out vec4 Color;

void main()
{
    Color = vec4(vnorm / 2.0 + vec3(0.5, 0.5, 0.5), 1.0);
}
