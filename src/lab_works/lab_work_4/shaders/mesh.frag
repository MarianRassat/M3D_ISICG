#version 450

in vec3 fragAmbientColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in float fragShininess;

in vec3 fragNormal;   // Normal
in vec3 fragPosition; // Frag position from the camera
in vec3 fragLight;	  // Light position from the camera

uniform int uBlinn;

layout( location = 0 ) out vec4 outColor;

void main()
{
	vec3 fragToLight = fragLight - fragPosition;
	vec3 fragToCam = - fragPosition;

	// make the normal face the light
	vec3 r_fragNormal = dot( fragNormal , normalize( fragToCam ) ) < 0 ? -fragNormal : fragNormal ; 


	vec3 ambientComponent = fragAmbientColor;
	vec3 diffuseComponent = fragDiffuseColor * max( dot( normalize( fragToLight ), r_fragNormal ), 0 );
	vec3 specularComponent;

	switch (uBlinn) {
	case 0: // None
		specularComponent = vec3(0);
		break;

	case 1: // Phong
		specularComponent = fragSpecularColor * pow( max( dot( normalize( reflect( -fragToLight, r_fragNormal ) ), normalize( -fragPosition ) ), 0 ), fragShininess );
		break;

	case 2: // Blinn
		vec3 half_vec =  normalize( normalize( fragToLight ) + normalize( -fragPosition ) ); // H = normalize(L + V)
		specularComponent = fragSpecularColor * pow( max( dot( normalize( half_vec ), r_fragNormal ), 0 ), fragShininess );
		break;

	}

	outColor = vec4( ambientComponent 
					 + diffuseComponent
					 + specularComponent
					 , 1.f );

}
