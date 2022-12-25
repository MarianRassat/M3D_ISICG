#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uVPDepthMatrix; // Projection * View
uniform mat4 uMDepthMatrix; //                      Model

void main() {
	
	gl_Position = uMDepthMatrix * vec4( aVertexPosition, 1.f );
	gl_Position = uVPDepthMatrix * gl_Position;

}
