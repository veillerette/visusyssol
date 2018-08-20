#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;
in mat4 vNormalMatrix;

layout (location = 0) out vec4 fFragColor;
layout (location = 1) out vec4 fBrightColor;

uniform vec3 uLightDir_vs;
uniform vec3 uColor;

vec3 blinnPhong()
{
	/*vec3 w0 = normalize(-vPosition_vs);
	vec3 halfv = normalize((w0 - uLightDir_vs) / 2);

	return vec3(1.0, 1.0, 1.0) * (normalize(vec3(0.5075, 0.5075, 0.5075)) *
		(dot(normalize(uLightDir_vs),normalize(vNormal_vs))) +
		normalize(vec3(0.508273, 0.508273, 0.508273)) * (pow(dot(halfv, normalize(vNormal_vs)), 12)));
		*/

    vec3 w0 = normalize(-vPosition_vs);
    vec3 wi = normalize(uLightDir_vs-vPosition_vs);
    vec3 halfv = normalize((w0 + wi) / 2);

    return vec3(1.0, 1.0, 1.0) * (normalize(vec3(0.5075, 0.5075, 0.5075)) *
    		(dot(normalize(wi),normalize(vNormal_vs))) +
    		normalize(vec3(0.508273, 0.508273, 0.508273)) * (pow(dot(halfv, normalize(vNormal_vs)), 0.4f)));
}

void main() {
    vec3 luma = (1-blinnPhong());
    if(dot(luma, vec3(0.333, 0.333, 0.333)) > 0.80) {
        fFragColor = vec4(0,0,0,0);
	    fBrightColor = vec4(0.00,0.00,0.0,1);
    } else {
        fFragColor = vec4(luma * uColor, 0.15);
        fBrightColor = vec4(normalize(uColor)/15,1);
	}
}
