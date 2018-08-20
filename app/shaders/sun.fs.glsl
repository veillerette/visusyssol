#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

layout (location = 0) out vec3 fFragColor;
layout (location = 1) out vec3 fBrightColor;

uniform vec3 uColor;
uniform int uTime;

void main() {
    int bias = ((uTime/30) % (100));
    if(bias < 50) {
        bias -= 0; // to test other values
    } else {
        bias = -bias+100;
    }
    fFragColor = vec3(40+bias, 40+bias, 15+bias); // over values to simulate over effect of the gaussian blur.
    fBrightColor = vec3(fFragColor);
}
