#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uMVMatrix;  //              View * Model
uniform mat4 uVMatrix;   //				 View

uniform mat4 uNormalMatrix;

uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;

uniform vec3 uLightPosition;

out vec3 fragAmbientColor;
out vec3 fragDiffuseColor;
out vec3 fragSpecularColor;
out float fragShininess;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragLight;


void main()
{
	
	fragAmbientColor = uAmbientColor;
	fragDiffuseColor = uDiffuseColor;
	fragSpecularColor = uSpecularColor;
	fragShininess = uShininess;


	vec4 t_fragNormal = uNormalMatrix * vec4( normalize( aVertexNormal ), 1.);
	fragNormal = normalize( vec3( t_fragNormal / t_fragNormal.w ) );
	
	vec4 t_fragPosition = uMVMatrix * vec4( aVertexPosition, 1. );
	fragPosition = vec3( t_fragPosition / t_fragPosition.w ); // position du frag dans le plan camera

	vec4 t_fragLightPosition = uVMatrix * vec4( uLightPosition, 1. ); // calculer la position de la light dans le plan camera
	fragLight = vec3( t_fragLightPosition / t_fragLightPosition.w );  // vecteur frag->light

	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );

}
