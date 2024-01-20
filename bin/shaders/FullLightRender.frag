#version 330

in vec3 vcol;
in vec2 vuv;
in float height;
in vec4 shadowCoord;


// Направление освещения относительно камеры
in vec3 lightDirCam;
// Направление взгляда относительно камеры
in vec3 eyeDirCam;
// Позиция нормали вершины относительно камеры
in vec3 normCam;
// Позиция вершины относительно начала координат сцены
in vec3 posWorld;
// Глубина точки
in float pointDepth;


uniform sampler2D grass;
uniform sampler2D snow;
uniform sampler2D rocks;
uniform sampler2D sand;

uniform sampler2DShadow shadowMap;

out vec4 Color;

vec4 getTexColor()
{
    vec4 GrassColor = texture(grass, vuv.st);
    vec4 SnowColor = texture(snow, vuv.st);
    vec4 RocksColor = texture(rocks, vuv.st);
    vec4 SandColor = texture(sand, vuv.st);

    if (height < 10)
    {
        return SandColor;
    }
    else if (height < 30)
    {
        return mix(SandColor, GrassColor, clamp((height - 10) / (20-10), 0.0, 1.0));
    }
    else if (height < 50)
    {
        return mix(GrassColor, RocksColor, clamp((height - 30) / (50 - 30), 0.0, 1.0));
    }
    else if (height < 70)
    {
        return mix(RocksColor, SnowColor, clamp((height - 50) / (70 - 50), 0.0, 1.0));
    }
    else
    {
        return SnowColor;
    }

}


void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float lightPower = 1.0f;

    vec3 diffuse = getTexColor().rgb;
    vec3 ambient = vec3(0.1, 0.1, 0.1) * diffuse;

    vec3 n = normalize(normCam);
    vec3 l = normalize(lightDirCam);

    float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    float bias = 0.0;

    float visibility = 0.2 + 0.8 * texture(shadowMap, vec3(shadowCoord.xy, (shadowCoord.z - bias)/shadowCoord.w));

    vec4 fogColor = vec4(0.8, 0.8, 0.8, 1.0);
    float fog = clamp(1.0 - exp( - pow(pointDepth * 0.005, 2)), 0.0, 1.0);


    Color = mix(vec4(ambient + diffuse * lightColor * lightPower * cosTheta * visibility
    , 1.0),
    fogColor,
    fog);
}
