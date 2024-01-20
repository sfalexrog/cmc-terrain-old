#version 330

in vec3 uv;
out vec4 Color;

uniform samplerCube skybox;

void main()
{
    Color = texture(skybox, uv);
}
