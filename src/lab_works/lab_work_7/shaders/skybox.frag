#version 450

in vec3 fragPosition;

layout( binding = 0 ) uniform samplerCube uSkybox;
uniform vec3 uCameraPos;

layout( location = 0 ) out vec3 outPosition;
layout( location = 1 ) out vec3 outNormal;
layout( location = 2 ) out vec3 outAmbient;
layout( location = 3 ) out vec3 outDiffuse;
layout( location = 4 ) out vec4 outSpecular;

void main() {

	outAmbient = texture(uSkybox, fragPosition).xyz;
	outPosition = fragPosition * 4 + uCameraPos;
	outSpecular = vec4(vec3(0.), -1);

}
