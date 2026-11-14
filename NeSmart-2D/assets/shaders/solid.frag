#version 330 core

uniform vec3 uColor;  // flat colour of the whole shape

out vec4 fragColor;

void main()
{
    fragColor = vec4(uColor, 1.0);
}
