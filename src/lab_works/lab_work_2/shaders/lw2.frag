#version 450

in vec4 fragColor;

layout( location = 0 ) out vec4 color;

uniform float uOpacity;

void main() {

	color = fragColor * uOpacity;

}
