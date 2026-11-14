#version 330 core

layout(location = 0) in vec2 aPos;  // vertex position in the mesh's own space (unit quad: 0..1)
layout(location = 1) in vec2 aUV;   // texture coordinate

uniform mat4 uProjection;  // world -> clip space (Camera)
uniform mat4 uModel;       // mesh space -> world space (position and size of this sprite)

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
