#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;
uniform mat4 m, v, p;

out vec3 color;

void main() {
    color = aColor;
    gl_Position = p * v * m * vec4(aPosition, 1.0f);
}
