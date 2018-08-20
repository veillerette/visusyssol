#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

layout (location = 0) out vec3 fFragColor;
layout (location = 1) out vec3 fBrightColor;

uniform vec3 uColor;

void main() {
    fFragColor = uColor;
    fBrightColor = vec3(0,0,0);
}
