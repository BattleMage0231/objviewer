#version 330 core

in vec3 pos;
in vec3 normal;
in float matId;

#define MAX_MATERIALS 100
uniform vec3 matKa[MAX_MATERIALS];
uniform vec3 matKd[MAX_MATERIALS];
uniform vec3 matKs[MAX_MATERIALS];
uniform vec3 matKe[MAX_MATERIALS];
uniform float matNs[MAX_MATERIALS];
uniform float matD[MAX_MATERIALS];

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 color;

void main() {
    int id = int(matId + 0.5);

    vec3 ambient = 0.3 * matKa[id];

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * matKd[id];

    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = 1.0 * pow(max(dot(viewDir, reflectDir), 0.0), matNs[id]) * matKs[id];

    vec3 emissive = matKe[id];

    vec3 result = ambient + diffuse + specular + emissive;
    vec3 gammaCorrected = pow(result, vec3(1.0 / 2.2));

    color = vec4(gammaCorrected, matD[id]);
}
