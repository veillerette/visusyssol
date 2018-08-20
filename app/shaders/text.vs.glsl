#version 330 core

layout (location = 0) in vec2 aVertexPosition;
layout (location = 1) in vec2 aVertexNormal;
layout (location = 2) in vec2 aVertexTexCoords;

out vec2 vTexCoords;

uniform int uX;
uniform int uY;
uniform int W;
uniform int H;


void main()
{
    int w2 = W/2;
    int h2 = H/2;
    vTexCoords = aVertexTexCoords;
    gl_Position = vec4((aVertexPosition.x+uX-w2)/w2, (aVertexPosition.y+uY-h2)/h2, 0.0, 1.0);
}

