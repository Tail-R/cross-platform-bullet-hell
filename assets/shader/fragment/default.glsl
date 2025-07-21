#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D ourTexture;
uniform float time;

void main() {
    float _time = time * 0.0;

    FragColor = texture(ourTexture, TexCoord);
}

