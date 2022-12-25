#version 450

layout( location = 0 ) in vec2 aVertexPosition;

void main() {

	gl_Position = vec4(aVertexPosition, 0f, 1.f);

}
