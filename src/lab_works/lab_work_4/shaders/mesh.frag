#version 450

in vec3 fragAmbientColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in float fragShininess;

in vec3 fragNormal;   // Normal
in vec3 fragPosition; // Frag position from the camera
in vec3 fragLight;	  // Light position from the camera

in flat int fragBlinn;

layout( location = 0 ) out vec4 outColor;

void main()
{
	// compute the vector from the frag to the light
	vec3 fragToLight = fragLight - fragPosition;

	// make the normal face the light
	vec3 r_fragNormal = dot( fragNormal , normalize( fragToLight ) ) < 0 ? -fragNormal : fragNormal ; 


	vec3 ambient_component = fragAmbientColor;
	vec3 diffuse_component = fragDiffuseColor	*      max( dot( normalize( fragToLight ), r_fragNormal ), 0 );
	vec3 specular_component;

	switch (fragBlinn) {
	case 0: // None
		specular_component = vec3(0);
		break;

	case 1: // Phong
		specular_component = fragSpecularColor * pow( max( dot( normalize( reflect( -fragToLight, r_fragNormal ) ), normalize( -fragPosition ) ), 0 ), fragShininess );
		break;

	case 2: // Blinn
		vec3 half_vec =  normalize( normalize( fragToLight ) + normalize( -fragPosition ) ); // H = normalize(L + V)
		specular_component = fragSpecularColor * pow( max( dot( normalize( half_vec ), r_fragNormal ), 0 ), fragShininess );
		break;

	}


	outColor = vec4( ambient_component 
					 + diffuse_component
					 + specular_component
					 , 1.f );

}
