#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexColor;

uniform mat4 uTransformMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

out vec4 fragColor;

void main() {

	fragColor = vec4(aVertexColor, 1.f);

	gl_Position = uProjMatrix * uViewMatrix * uTransformMatrix * vec4(aVertexPosition, 1.f);

}
