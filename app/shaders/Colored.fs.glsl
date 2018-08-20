#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;
in vec3 vColor;

layout (location = 0) out vec3 fFragColor;
layout (location = 1) out vec3 fBrightColor;

void main() {
    fFragColor = vColor;
    fBrightColor = vec3(0,0,0);
}
