#version 330

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 DepthMVP;

uniform vec3 lightDirInverse;
out vec3 lightDirCam;
out vec3 normCam;

void main()
{

    lightDirCam = (view * vec4(lightDirInverse, 0.0)).xyz;
    normCam = (view * model * vec4(norm, 0.0)).xyz;

	//gl_Position = projection * view * model * vec4(pos, 1.0);
	gl_Position = DepthMVP * vec4(pos, 1.0);
}
