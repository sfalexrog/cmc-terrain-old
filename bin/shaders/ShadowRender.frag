#version 330

in vec3 vcol;
in vec2 vuv;
in float height;
in vec4 shadowCoord;

uniform sampler2D grass;
uniform sampler2D snow;
uniform sampler2D rocks;
uniform sampler2D sand;

uniform sampler2DShadow shadowMap;

out vec4 Color;

void main()
{
    vec4 GrassColor = texture(sand, vuv.st);
    vec4 SnowColor = texture(snow, vuv.st);
    float heightNormalized = height / 100;

    float bias = 0.05;

    float visibility = 0.2 + 0.8 * texture(shadowMap, vec3(shadowCoord.xy, (shadowCoord.z - bias)/shadowCoord.w));

    Color = visibility * mix(GrassColor, SnowColor, clamp((heightNormalized - 0.3) / (0.7 - 0.3), 0.0, 1.0));
}
