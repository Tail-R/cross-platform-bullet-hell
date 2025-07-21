#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D ourTexture;
uniform float time;

void main() {
    float _time = time * 0.0;

    vec4 texColor = texture(ourTexture, TexCoord);

    float alpha = 0.6;
    vec3 darkened = mix(texColor.rgb, vec3(0.0), 0.5);

    FragColor = vec4(darkened, texColor.a * alpha);
}

