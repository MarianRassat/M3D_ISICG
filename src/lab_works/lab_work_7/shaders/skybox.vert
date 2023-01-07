#version 450

layout( location = 0 ) in vec3 aVertexPosition;

out vec3 fragPosition;

uniform mat4 uVPMatrix;

void main() {

    fragPosition = aVertexPosition;

    gl_Position = uVPMatrix * vec4(aVertexPosition, 1.);

}  