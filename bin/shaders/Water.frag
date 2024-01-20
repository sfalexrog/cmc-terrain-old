#version 330

in vec4 clipSpace;
in vec2 dudvCoord;
in vec3 toCamera;
in vec3 lightDirCam;
in vec3 eyeDirCam;

out vec4 Color;

uniform sampler2D reflection;
uniform sampler2D refraction;

uniform sampler2D dudv;

uniform sampler2D normalMap;
uniform mat4 view;
uniform mat4 model;

uniform float time;

const float strength = 0.02;
const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main()
{
    float delta = time / 25.0;

    vec2 devCoords = (1.0 + clipSpace.xy / clipSpace.w) / 2.0;

    vec2 refractCoord = vec2(devCoords.x, devCoords.y);
    vec2 reflectCoord = vec2(devCoords.x, 1.0-devCoords.y);

    vec2 distortedTexCoords = texture(dudv, vec2(dudvCoord.x + delta, dudvCoord.y)).rg * 0.1;
    distortedTexCoords = dudvCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + delta);
    vec2 distortion = (texture(dudv, distortedTexCoords).rg * 2.0 - 1.0) * strength;

    reflectCoord = clamp(reflectCoord + distortion, vec2(0.001, 0.001), vec2(0.999, 0.999));
    refractCoord = clamp(refractCoord + distortion, vec2(0.001, 0.001), vec2(0.999, 0.999));

    vec4 reflectCol = texture(reflection, reflectCoord);
    vec4 refractCol = texture(refraction, refractCoord);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec3 viewVec = normalize(toCamera);
    float refractiveFactor = dot(viewVec, vec3(0.0, 0.0, 1.0));
    refractiveFactor = pow(refractiveFactor, 0.5);

    Color = mix(reflectCol, refractCol, refractiveFactor);
    Color = mix(Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}

