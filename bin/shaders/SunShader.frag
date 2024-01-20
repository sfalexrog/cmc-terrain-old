#version 330

in vec2 vpos;

uniform float sunSize;

uniform vec3 sunColor;

out vec4 Color;

void main()
{
    float distance = sqrt(vpos.x * vpos.x + vpos.y * vpos.y);
    float alpha;
    if (distance < 0.25 * sunSize)
    {
        alpha = 1.0;
    }
    else
    {
        alpha = clamp(1.0 - pow((distance - 0.25 * sunSize)/(0.1 * sunSize), 2), 0.0, 1.0);
    }

    Color = vec4(sunColor, alpha);
}
