#version 330 core

layout(location = 0) in vec2 aPos;  // unit quad: 0..1
layout(location = 1) in vec2 aUV;   // unit quad texture coordinate

uniform mat4 uProjection;  // world -> clip space (Camera)
uniform mat4 uModel;       // unit quad -> this glyph's rectangle in world space
uniform vec2 uUVOffset;    // glyph's place in the font atlas: uv = uUVOffset + aUV * uUVSize
uniform vec2 uUVSize;

out vec2 vUV;

void main()
{
    vUV = uUVOffset + aUV * uUVSize;
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
