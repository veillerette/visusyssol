#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

layout (location = 0) out vec3 fFragColor;
layout (location = 1) out vec3 fBrightColor;

uniform sampler2D uTexture;

void main() {
    vec3 color =  vec3(texture(uTexture, vTexCoords));
    float i = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(i < 0.2) {
        fFragColor = vec3(0,0,0);
        fBrightColor = vec3(0,0,0);
    } else {
        fFragColor = vec3(i,i,i);
        if(i > 0.5) {
            fBrightColor = vec3(i,i,i) * 4;
        } else {
            fBrightColor = vec3(0,0,0);
        }
    }
}
