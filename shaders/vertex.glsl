#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 view;
uniform mat4 projection;

out vec4 fragColor;

void main() {
    gl_Position = projection * view * vec4(pos, 1.0f);
    fragColor = vec4(color, 1.0f);
}
