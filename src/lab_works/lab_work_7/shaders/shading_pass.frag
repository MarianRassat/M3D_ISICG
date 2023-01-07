#version 450

// Texture maps
layout ( binding = 0 ) uniform sampler2D uPosition;
layout ( binding = 1 ) uniform sampler2D uNormal;
layout ( binding = 2 ) uniform sampler2D uAmbient;
layout ( binding = 3 ) uniform sampler2D uDiffuse;
layout ( binding = 4 ) uniform sampler2D uSpecular;

layout ( binding = 5 ) uniform sampler2D uShadowMap;

// matrices
uniform mat4 uVP;
uniform mat4 uDepthVP; // Only View Proj because model is aldready applied to uPosition

// Vectors
uniform vec3 uLight;
uniform vec3 uCamera;
uniform vec3[128] uSSAOSamples;
uniform int uSSAOSampleSize;

// Options
uniform int uSpecularType;

layout( location = 0 ) out vec4 outColor;

float smoothShadow(vec4 shadowCoords, float depthBias) {

	vec2 texelSize = 1. / textureSize( uShadowMap, 0 );
	float shadow = 0;

	// sample points around the designated coordinates
	float total_shadows = 0;
	for (float x_bias = -1; x_bias <= 1; x_bias++) {
		for (float y_bias = -1; y_bias <= 1; y_bias++) {
			total_shadows++;

			vec2 tex_position = shadowCoords.xy + vec2(x_bias, y_bias) * texelSize;
			
			if (!(tex_position.x < 0 || tex_position.x > 1 || tex_position.y < 0 || tex_position.y > 1)) {
				float depth = texture( uShadowMap, tex_position ).x;
				shadow += shadowCoords.z - depthBias < depth ? 1 : 0;
			}
		
		}
	}

	shadow /= total_shadows;

	return shadow;
}

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

		float fragShadowMap = texelFetch( uShadowMap, coords * 3, 0 ).x / 5;

		bool fragIsSkybox = fragShininess == -1;

	// parameters
		vec3 fragToLight    = uLight - fragPosition  ;
		vec3 fragToLightVec = normalize( fragToLight   );
		vec3 fragToCam   = uCamera - fragPosition;
		vec3 fragToCamVec   = normalize( fragToCam );

	// make the normal face the CAMERA
		fragNormal = dot( fragNormal , fragToCamVec ) < 0 ? -fragNormal : fragNormal;
	


// ========================= Compute lighting
	vec3 ambientComponent;
	vec3 specularComponent;
	vec3 diffuseComponent;

	// ambient color
		ambientComponent = fragIsSkybox ? fragAmbient : fragDiffuse * 0.25;

	// diffuse component
		if (!fragIsSkybox) {
			// consider the normal / light cos(angle) for the diffuse component ( Phong )
			diffuseComponent = fragDiffuse * max( dot( fragToLightVec, fragNormal ), 0 );
		}
	// specular component
	
		if (!fragIsSkybox) {
			specularComponent = fragSpecular;
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
		}


// ========================= Shadow map

	vec4 shadowCoords = uDepthVP * vec4(fragPosition, 1.);
	shadowCoords /= shadowCoords.w; // normalize for perspective
	shadowCoords = shadowCoords * 0.5 + 0.5; // get back to [0, 1]

	float depthBias = 0.0001 + pow( length(fragToCam)/2000, 2.5 );

	float realDistance = shadowCoords.z;
	
	float shadowMinDepthForFrag = texture( uShadowMap, shadowCoords.xy ).x;
	if (shadowCoords.x < 0 || shadowCoords.x > 1 || shadowCoords.y < 0 || shadowCoords.y > 1) {
		shadowMinDepthForFrag = 0;
	}

	float shadow = 0;

	bool smoothenShadow = true;
	
	if ( smoothenShadow ) {
		shadow = smoothShadow(shadowCoords, depthBias);
	} else {
		shadow += shadowCoords.z - depthBias < shadowMinDepthForFrag ? 1 : 0;
	}


// ========================= God rays

	vec3 godray_color = vec3(0.08, 0.08, 0.12);
	float godray_nb = 0;
	vec2 texelSize = 1. / textureSize( uShadowMap, 0 );
	
	// sample points along the camera -> frag line
	float steps = 100;

	for (float i = 0; i < steps; i++) {

		float t = i / steps;
		vec3 point = (1.-t) * uCamera + t * fragPosition;
		

		vec4 pt_shadow = uDepthVP * vec4(point, 1.);
		pt_shadow /= pt_shadow.w; // normalize for perspective
		pt_shadow = pt_shadow * 0.5 + 0.5; // get back to [0, 1]


		if (!(pt_shadow.x < 0 || pt_shadow.x > 1 || pt_shadow.y < 0 || pt_shadow.y > 1)) {
			float depth = texture( uShadowMap, pt_shadow.xy ).x;
			godray_nb += pt_shadow.z - depthBias < depth ? 1 : 0;
		}


	}

	float godray_factor = smoothstep( 0, steps, godray_nb );
	
// ========================= SSAO

	float occlusion = 0.0;
	if (!fragIsSkybox) {

		float SSAOBias = 0.00005;

		float maxLengthSample = 0.1;


		for(int i = 0; i < uSSAOSampleSize; ++i)
		{
			vec3 samplePosWorld = fragPosition + uSSAOSamples[i] * maxLengthSample;
    
			// get clip space position, to in turn get depth
			vec4 samplePos = uVP * vec4(samplePosWorld, 1.);
			samplePos /= samplePos.w;						// take into account perspective
			// if the pixel isn't part of the skybox then get the depth
			vec2 sampleTexCoords = samplePos.xy * 0.5 + 0.5;
			if ( texture( uSpecular, sampleTexCoords ).w != -1 ) {
				vec3 sampleTruePosWorld = texture(uPosition, (samplePos * 0.5 + 0.5).xy).xyz;
				vec4 sampleTruePos = uVP * vec4( sampleTruePosWorld, 1.);
				sampleTruePos /= sampleTruePos.w; // take into account perspective

				float sampleTrueDepth = sampleTruePos.z;
				float sampleDepth = samplePos.z;

				float sampleTrueDist = length(sampleTruePosWorld - uCamera);
				float sampleDist	 = length(samplePosWorld     - uCamera);

				float rangeFactor = smoothstep(0., 1., maxLengthSample / abs(sampleTrueDist - sampleDist) );

				occlusion += (sampleTrueDepth <= sampleDepth - SSAOBias ? 1. : 0.) * rangeFactor;
			}
		}
		occlusion /= uSSAOSampleSize;
	}
	occlusion = 1 - occlusion;

// ========================= Results
	outColor = vec4( ambientComponent * occlusion
					+ diffuseComponent * shadow
					+ specularComponent * shadow
				    + godray_factor * godray_color
					, 1. );

}
