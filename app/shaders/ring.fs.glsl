#version 330

in vec3 vPosition_vs;
in vec2 vTexCoords;

out vec4 fFragColor;

uniform sampler2D uTexture;
uniform sampler2D uTextureAlpha;

void main() {
    fFragColor = vec4(vec3(texture(uTexture, vTexCoords)), dot(vec3(texture(uTextureAlpha, vTexCoords)), vec3(0.33, 0.33, 0.33)));
}
