#version 450

in vec4 fragColor;

layout( location = 0 ) out vec4 outColor;

uniform float uOpacity;

void main() {

	// take into account opacity
	outColor = fragColor * uOpacity;

}
