#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in float inMatId;

uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;
out float matId;

void main() {
    gl_Position = projection * view * vec4(inPos, 1.0f);
    pos = inPos;
    normal = inNormal;
    matId = inMatId;
}
