#version 330 core

in vec2 vUV;

uniform sampler2D uTexture;  // font atlas, one channel: glyph coverage
uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    // Coverage becomes alpha, so the glyph edges blend with what is behind (needs GL_BLEND).
    fragColor = vec4(uColor, texture(uTexture, vUV).r);
}
