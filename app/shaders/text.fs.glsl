#version 330

in vec2 vTexCoords;

out vec4 fFragColor;

uniform sampler2D uTextureFont;

void main() {
    vec3 c =  vec3(texture(uTextureFont, vTexCoords));
    if(c.r > (c.g + c.b)/2+0.5) {
        fFragColor = vec4(0,0,1, 0);
    } else {
        c.r = (c.g + c.b) / 2;
        fFragColor = vec4(c, 1.0);
    }
}
