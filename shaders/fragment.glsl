#version 330 core

in vec3 pos;
in vec3 normal;
in float matId;
in float groupId;

#define MAX_MATERIALS 100
uniform vec3 matKa[MAX_MATERIALS];
uniform vec3 matKd[MAX_MATERIALS];
uniform vec3 matKs[MAX_MATERIALS];
uniform vec3 matKe[MAX_MATERIALS];
uniform float matNs[MAX_MATERIALS];
uniform float matD[MAX_MATERIALS];

#define MAX_GROUPS 200
uniform int groupVisibility[MAX_GROUPS];
uniform int selectedGroup;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float emissiveStrength;
uniform int applyGamma;

out vec4 color;

vec4 getColor() {
    int id = int(matId + 0.5);

    vec3 ambient = ambientStrength * matKa[id];

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 diffuse = diffuseStrength * max(dot(norm, lightDir), 0.0) * matKd[id];

    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), matNs[id]) * matKs[id];

    vec3 emissive = emissiveStrength * matKe[id];

    return vec4(ambient + diffuse + specular + emissive, matD[id]);
}

void main() {
    int id = int(matId + 0.5);
    int gid = int(groupId + 0.5);
    if(groupVisibility[gid] == 0) discard;
    
    if(gid == selectedGroup) {
        color = vec4(0.4, 0.4, 0.4, 1.0);
    } else {
        color = getColor();
    }

    if(applyGamma == 1) {
        vec3 gammaCorrected = pow(color.xyz, vec3(1.0 / 2.2));
        color = vec4(gammaCorrected, color.w);
    }
}
