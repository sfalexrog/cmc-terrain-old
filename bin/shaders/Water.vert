#version 330

in vec3 pos;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec3 camPosition;
uniform vec3 lightPosition;

out vec4 clipSpace;
out vec2 dudvCoord;
out vec3 toCamera;
out vec3 lightDirCam;
out vec3 eyeDirCam;


const float tiles = 64.0;


void main()
{
    vec4 worldPos = model * vec4(pos, 1.0f);
    clipSpace = projection * view * model * vec4(pos, 1.0f);
    gl_Position = clipSpace;
    dudvCoord = vec2(pos.x / 2.0 + 0.5, pos.y / 2.0 + 0.5) * tiles;
    toCamera = camPosition - worldPos.xyz;

    eyeDirCam = vec3(0.0, 0.0, 0.0) - (view * model * vec4(pos, 1.0)).xyz;

    lightDirCam = (view * vec4(lightPosition, 1.0)).xyz + eyeDirCam;

}

