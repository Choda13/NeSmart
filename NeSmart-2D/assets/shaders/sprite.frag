#version 330 core

in vec2 vUV;

uniform sampler2D uTexture;  // texture unit the sprite's image is bound to

out vec4 fragColor;

void main()
{
    // Keep the alpha channel so transparent parts of an image blend with what is behind (needs GL_BLEND).
    fragColor = texture(uTexture, vUV);
}
