#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform float time;

// ハッシュ
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// スムーズノイズ（quintic補間）
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

// FBMノイズ（4オクターブ）
float fbm(vec2 p) {
    float total = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; ++i) {
        total += noise(p) * amplitude;
        p *= 2.0;
        amplitude *= 0.5;
    }
    return total;
}

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);

    // オーラ領域
    float dist = distance(TexCoord, vec2(0.5));
    float baseAura = 1.0 - smoothstep(0.3, 0.55, dist);

    // FBMノイズによるゆらぎ
    vec2 flameCoord = TexCoord * 6.0 + vec2(0.0, time * 1.5);
    float flameNoise = fbm(flameCoord);

    // オーラの強さ（透明部分を中心に強調）
    float flameAura = baseAura * flameNoise * 4.0 * (1.0 - texColor.a);

    // 色合い：赤→橙
    vec3 flameColor = mix(vec3(0.0, 0.8, 0.6), vec3(0.0, 0.4, 0.0), flameNoise);

    vec3 finalColor = mix(texColor.rgb, flameColor, flameAura);
    float finalAlpha = max(texColor.a, flameAura * 0.4);

    FragColor = vec4(finalColor, finalAlpha);
}

