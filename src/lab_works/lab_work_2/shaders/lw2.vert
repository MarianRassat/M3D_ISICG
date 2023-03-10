#version 450

layout( location = 0 ) in vec2 aVertexPosition;
layout( location = 1 ) in vec3 aVertexColor;

out vec4 fragColor;

uniform float uTranslationX;

void main() {

	// pass color to the fragment shader
	fragColor	= vec4( aVertexColor, 1.f );

	// pass the position with the translation applied
	gl_Position = vec4( aVertexPosition.x + uTranslationX, aVertexPosition.y, 0.f, 1.f );

}
