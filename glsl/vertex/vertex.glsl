#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 offset;

void main() {
    gl_Position = vec4(aPos.xy + offset, aPos.z, 1.0f);
    TexCoord = aTexCoord;
}