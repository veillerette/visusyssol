#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;
in mat4 vNormalMatrix;

out vec3 fFragColor;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;
uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;

uniform sampler2D uTexture;

vec3 blinnPhong()
{
	vec3 w0 = normalize(-vPosition_vs);
	vec3 wi = normalize(uLightPos_vs-vPosition_vs);
	vec3 halfv = normalize((w0 + wi) / 2);
	float d = distance(uLightPos_vs, vPosition_vs) * 100;

	return normalize(uLightIntensity / (d*d)) * (normalize(uKd) *
		(dot(normalize(wi),normalize(vNormal_vs))) +
		normalize(uKs) * (pow(dot(halfv, normalize(vNormal_vs)), uShininess)));
}

void main() {
	fFragColor = blinnPhong();
}
