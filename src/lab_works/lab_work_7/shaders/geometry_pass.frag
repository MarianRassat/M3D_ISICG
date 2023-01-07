#version 450

// Colors
	in vec3 fragAmbientColor;
	in vec3 fragDiffuseColor;
	in vec3 fragSpecularColor;
	in float fragShininess;

// Texture maps
	layout( binding = 0 ) uniform sampler2D uAmbientMap;
	layout( binding = 1 ) uniform sampler2D uDiffuseMap;
	layout( binding = 2 ) uniform sampler2D uSpecularMap;
	layout( binding = 3 ) uniform sampler2D uShininessMap;
	layout( binding = 4 ) uniform sampler2D uNormalMap;
	
	uniform int uHasAmbientMap;
	uniform int uHasDiffuseMap;
	uniform int uHasSpecularMap;
	uniform int uHasShininessMap;
	uniform int uHasNormalMap;

	in vec2 fragTexCoords;


// Vectors
	in vec3 fragNormal;   // Normal
	in vec3 fragPosition; // Frag position from the camera (tangent space if uHasNormalMap = 1)
	in mat3 TBN;

// out components
	layout( location = 0 ) out vec3 outPosition;
	layout( location = 1 ) out vec3 outNormal;
	layout( location = 2 ) out vec3 outAmbient;
	layout( location = 3 ) out vec3 outDiffuse;
	layout( location = 4 ) out vec4 outSpecular;

void main()
{

// ========================= Init values

	float opacity = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCoords).w  : 1 ;
	// discard fragments with low opacity
	if (opacity < 0.5 ) discard;

	vec3 ambient_component  = uHasAmbientMap  == 1 ? texture(uAmbientMap,  fragTexCoords).xyz  : fragAmbientColor ;
	vec3 diffuse_component  = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCoords).xyz  : fragDiffuseColor ;
	vec3 specular_component = uHasSpecularMap == 1 ? texture(uSpecularMap, fragTexCoords).xxx  : fragSpecularColor ;

	// normal
	vec3 r_fragNormal = normalize( uHasNormalMap == 1 ? TBN * ( texture(uNormalMap, fragTexCoords).xyz * 2 - 1 ) : fragNormal ) ;

	// parameters
	float shininess_power = uHasShininessMap == 1 ? texture(uShininessMap, fragTexCoords).x : fragShininess;


// ========================= Results

	outPosition = fragPosition;
	outNormal = r_fragNormal;
	outAmbient = ambient_component;
	outDiffuse = diffuse_component;
	outSpecular = vec4(specular_component, shininess_power);

}
