#version 330 core

layout(location = 0) in vec2 aPos;  // vertex position in the mesh's own space

uniform mat4 uProjection;  // world -> clip space (Camera)
uniform mat4 uModel;       // mesh space -> world space (identity for meshes already built in world space)

void main()
{
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
