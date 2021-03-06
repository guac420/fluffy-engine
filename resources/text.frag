#version 430 core

uniform sampler2D uTexture;
uniform vec3 uColor;

in vec2 texCoord;
in vec3 color;

out vec4 frag_color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTexture, texCoord).r);
    frag_color = vec4(color, 1.0) * sampled;
}
