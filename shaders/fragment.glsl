#version 330 core

in vec3 worldPos;
in float matId;

#define MAX_MATERIALS 100
uniform vec3 matKd[MAX_MATERIALS];
uniform float matD[MAX_MATERIALS];

out vec4 color;

void main() {
    int id = int(matId + 0.5);
    color = vec4(matKd[id], matD[id]);
}
