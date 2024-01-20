#version 330

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float sunSize;

uniform vec3 sunPosition;

out vec2 vpos;


void main()
{

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertPos = sunPosition + camRight * pos.x * sunSize
    + camUp * pos.y * sunSize;

    mat4 VP = projection * view;

    gl_Position = VP * vec4(vertPos, 1.0);

    vpos = pos.xy;
}
