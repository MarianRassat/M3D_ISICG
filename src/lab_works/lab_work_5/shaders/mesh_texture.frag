#version 450

// Colors
	in vec3 fragAmbientColor;
	in vec3 fragDiffuseColor;
	in vec3 fragSpecularColor;
	in float fragShininess;

// Texture maps
	uniform int uHasAmbientMap;
	layout( binding = 0 ) uniform sampler2D uAmbientMap;
	uniform int uHasDiffuseMap;
	layout( binding = 1 ) uniform sampler2D uDiffuseMap;
	uniform int uHasSpecularMap;
	layout( binding = 2 ) uniform sampler2D uSpecularMap;
	uniform int uHasShininessMap;
	layout( binding = 3 ) uniform sampler2D uShininessMap;
	uniform int uHasNormalMap;
	layout( binding = 4 ) uniform sampler2D uNormalMap;

	in vec2 fragTexCoords;

// Vectors
	in vec3 fragNormal;   // Normal
	in vec3 fragPosition; // Frag position from the camera (tangent space if uHasNormalMap = 1)
	in vec3 fragLight;	  // Light position from the camera (tangent space if uHasNormalMap = 1)

// Options
	uniform int uSpecularType;

layout( location = 0 ) out vec4 outColor;

void main()
{

	// color components
	float opacity = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCoords).w  : 1 ;
	if (opacity < 0.5 ) discard;
	vec3 ambient_component  = uHasAmbientMap  == 1 ? texture(uAmbientMap,  fragTexCoords).xyz  : fragAmbientColor ;
	vec3 diffuse_component  = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCoords).xyz  : fragDiffuseColor ;
	vec3 specular_component = uHasSpecularMap == 1 ? texture(uSpecularMap, fragTexCoords).xxx  : fragSpecularColor ;

	// normal
	vec3 r_fragNormal = normalize( uHasNormalMap == 1 ? texture(uNormalMap, fragTexCoords).xyz * 2 - 1 : fragNormal ) ;

	// parameters
	float shininess_power = uHasShininessMap == 1 ? texture(uShininessMap, fragTexCoords).x : fragShininess;
	vec3 fragToLight = fragLight - fragPosition ;
	vec3 fragToCam =  - fragPosition ;

	// make the normal face the camera
	r_fragNormal = dot( r_fragNormal , normalize( fragToCam ) ) < 0 ? -r_fragNormal : r_fragNormal;
	r_fragNormal = normalize(r_fragNormal);
	
	// consider the normal / light cos(angle)
	diffuse_component *= max( dot( normalize( fragToLight ), r_fragNormal ), 0 );
	
	// apply the right specular type
	switch ( uSpecularType ) {
	case 0: // None
		specular_component = vec3(0);
		break;

	case 1: // Phong
		specular_component *= pow( max( dot( normalize( reflect( -fragToLight, r_fragNormal ) ), normalize( -fragPosition ) ), 0 ), shininess_power );
		break;

	case 2: // Blinn
		vec3 half_vec =  normalize( normalize( fragToLight ) + normalize( -fragPosition ) ); // H = normalize(L + V)
		specular_component *= pow( max( dot( normalize( half_vec ), r_fragNormal ), 0 ), shininess_power );
		break;

	}

	outColor = vec4( ambient_component 
					 + diffuse_component //* pow( 1 / length(fragToLight), 0.5 ) // lower light at a higher distance from the source
					 + specular_component //* pow( 1 / length(fragToLight), 2 )
					 , opacity );

}
