#version 330

in vec3 normCam;
in vec3 lightDirCam;

out float depth;

void main()
{
    depth = gl_FragCoord.z;
}
