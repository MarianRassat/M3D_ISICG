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

uniform vec3 uLightPosition; uniform int ufBlinn;

uniform int uHasNormalMap;

out vec3 fragAmbientColor;
out vec3 fragDiffuseColor;
out vec3 fragSpecularColor;
out float fragShininess;

out vec2 fragTexCoords;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragLight;

out flat int fragBlinn;


void main()
{
	
	fragAmbientColor = uAmbientColor;
	fragDiffuseColor = uDiffuseColor;
	fragSpecularColor = uSpecularColor;
	fragShininess = uShininess;

	fragTexCoords = aVertexTexCoords;

	fragBlinn = ufBlinn;

	vec3 N = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexNormal   ), 1.) ) );
	vec3 T = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexTangent  ), 1.) ) );
	T = normalize ( T - dot (T , N ) * N ); // On veut T orthogonal a N
	vec3 B = cross (N , T );

	mat3 inv_TBN = transpose( mat3(T, B, N) );
	
	if ( uHasNormalMap == 1 ) {
		fragPosition = inv_TBN * vec3( uMVMatrix * vec4( aVertexPosition, 1. ) );
		fragLight	 = inv_TBN * vec3( uVMatrix  * vec4( uLightPosition,  1. ) );
	} else {
		fragPosition = vec3( uMVMatrix * vec4( aVertexPosition, 1.) );
		fragLight	 = vec3( uVMatrix *  vec4( uLightPosition,  1.) );
	}


	fragNormal	 = N;

	gl_Position  = uMVPMatrix * vec4( aVertexPosition, 1.f );


}
