#version 450

layout( location = 0 ) in vec2 aVertexPosition;

void main() {

	// the position is left unchanged, with only z and w filled with 0 and 1
	gl_Position = vec4(aVertexPosition, 0f, 1.f);

}
