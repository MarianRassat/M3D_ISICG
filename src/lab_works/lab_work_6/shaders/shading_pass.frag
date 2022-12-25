#version 450

// Texture maps
layout ( binding = 0 ) uniform sampler2D uPosition;
layout ( binding = 1 ) uniform sampler2D uNormal;
layout ( binding = 2 ) uniform sampler2D uAmbient;
layout ( binding = 3 ) uniform sampler2D uDiffuse;
layout ( binding = 4 ) uniform sampler2D uSpecular;

// Vectors
uniform vec3 uLight; // Light position from the camera

// Options
uniform int uSpecularType;

layout( location = 0 ) out vec4 outColor;

void main()
{
// ========================= Init values

	// texel coords
		ivec2 coords = ivec2( gl_FragCoord.xy );

	// get the params for current frag from the gBuffer
		vec3  fragPosition	= texelFetch( uPosition, coords, 0 ).xyz;
		vec3  fragNormal	= texelFetch( uNormal,   coords, 0 ).xyz;
		vec3  fragAmbient	= texelFetch( uAmbient,  coords, 0 ).xyz;
		vec3  fragDiffuse	= texelFetch( uDiffuse,  coords, 0 ).xyz;
		vec3  fragSpecular	= texelFetch( uSpecular, coords, 0 ).xyz;
		float fragShininess = texelFetch( uSpecular, coords, 0 ).w;

	// parameters
		vec3 fragToLight    = uLight - fragPosition  ;
		vec3 fragToLightVec = normalize( fragToLight   );
		vec3 fragToCamVec   = normalize( -fragPosition );

	// make the normal face the CAMERA
		fragNormal = dot( fragNormal , fragToCamVec ) < 0 ? -fragNormal : fragNormal;
	


// ========================= Compute lighting

	// ambient color
		vec3 ambientComponent = fragAmbient;

	// diffuse component
		// consider the normal / light cos(angle) for the diffuse component ( Phong )
		vec3 diffuseComponent = fragDiffuse * max( dot( fragToLightVec, fragNormal ), 0 );
	
	// specular component
		vec3 specularComponent = fragSpecular;
		// apply the right specular type
		switch ( uSpecularType ) {
		case 0: // None
			specularComponent = vec3(0);
			break;

		case 1: // Phong
			specularComponent *= pow( max( dot( reflect( -fragToLightVec, fragNormal ), fragToCamVec ), 0 ), fragShininess );
			break;

		case 2: // Blinn
			// the half vector is in the middle of the frag->camera / frag->light vectors
			vec3 half_vec		=  normalize( fragToLightVec + fragToCamVec ); // H = normalize(L + V)
			// consider its cos( angle ) with the normal
			specularComponent  *= pow( max( dot( normalize( half_vec ), fragNormal ), 0 ), fragShininess );
			break;

		}


// ========================= Results

	outColor = vec4( ambientComponent 
					 + diffuseComponent
					 + specularComponent,
					 1. );

}
