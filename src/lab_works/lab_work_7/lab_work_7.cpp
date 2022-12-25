#include "lab_work_7.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb/stb_image.h"
#include "glm/gtx/string_cast.hpp"

namespace M3D_ISICG
{

	const std::string LabWork7::_shaderFolder = "src/lab_works/lab_work_7/shaders/";

	LabWork7::~LabWork7() {

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _shadingPassProgram );
		glDeleteProgram( _geometryPassProgram );

	}

	bool LabWork7::init()
	{

		std::cout << "Initializing lab work 7..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_mesh.load( "House", "./data/models/small_house.obj" );
		_mesh._transformation = glm::scale( _mesh._transformation, Vec3f(0.2, 0.2, 0.2) );

		glEnable( GL_DEPTH_TEST );

		_skyboxCubemap = loadCubemap( { "" } );

		_geometryPassProgram  = glCreateProgram();
		_shadingPassProgram	  = glCreateProgram();
		_shadowMappingProgram = glCreateProgram();
		_lightLookAt		  = Vec3f( 0, 0, 0 );

		_initCamera();

		_initSkyboxVAO();

		if ( !_initGBuffer() ) // good
			return false;
		
		if ( !_initShaders() )
			return false;

		if ( !_initGeometryPass() )
			return false;

		if ( !_initShadingPass() )
			return false;
		
		if ( !_initShadowMapping() )
			return false;

		std::cout << "Done!" << std::endl;
		return true;
	}

	bool LabWork7::_initSkyboxPass() {
		// init le program etc -> pass de la skybox
		// draw la skybox dans le fbo du gbuffer
		// puis après draw le reste etc
	}

	bool LabWork7::_initGeometryPass() {

		_uMVP		   = glGetUniformLocation(  _geometryPassProgram, "uMVPMatrix" );
		_uM			   = glGetUniformLocation(  _geometryPassProgram, "uMMatrix" );
		_uNormalMatrix = glGetUniformLocation(  _geometryPassProgram, "uNormalMatrix" );

		return true;

	}

	bool LabWork7::_initSkyboxVAO() {

		std::vector<Vec3f> points;
		std::vector<Vec3i> triangles;
		soifjz
		points.push_back( Vec3f( -1, -1, -1 ) );
		points.push_back( Vec3f( -1, -1,  1 ) );
		points.push_back( Vec3f(  1, -1,  1 ) );
		points.push_back( Vec3f(  1, -1, -1 ) );

		points.push_back( Vec3f( -1,  1, -1 ) );
		points.push_back( Vec3f( -1,  1,  1 ) );
		points.push_back( Vec3f(  1,  1,  1 ) );
		points.push_back( Vec3f(  1,  1, -1 ) );

		triangles.push_back( Vec3i( 0, 1, 2 ) );
		triangles.push_back( Vec3i( 0, 2, 3 ) );

		triangles.push_back( Vec3i( 0, 4, 7 ) );
		triangles.push_back( Vec3i( 0, 3, 7 ) );

		triangles.push_back( Vec3i( 0, 1, 5 ) );
		triangles.push_back( Vec3i( 0, 4, 5 ) );

		triangles.push_back( Vec3i( 1, 2, 6 ) );
		triangles.push_back( Vec3i( 1, 5, 6 ) );
		
		triangles.push_back( Vec3i( 2, 3, 7 ) );
		triangles.push_back( Vec3i( 2, 6, 7 ) );

		triangles.push_back( Vec3i( 4, 5, 6 ) );
		triangles.push_back( Vec3i( 4, 7, 6 ) );
		

		glCreateBuffers( 1, &_vboSkybox );
		glNamedBufferData( _vboSkybox, points.size() * sizeof( Vec3f ), points.data(), GL_STATIC_DRAW );

		// Create VAO
		glCreateVertexArrays( 1, &_vaoSkybox );
		glEnableVertexArrayAttrib( _vaoSkybox, 0 ); // Enable attribute at index 0

		glVertexArrayAttribFormat(
			_vaoSkybox, 0, 3, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 2 space between each object

		glVertexArrayVertexBuffer( _vaoSkybox, 0, _vboSkybox, 0, sizeof( Vec3f ) );

		glVertexArrayAttribBinding( _vaoSkybox, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao

		glCreateBuffers( 1, &_eboSkybox );
		glNamedBufferData( _eboSkybox, _triangles.size() * sizeof( Vec3i ), _triangles.data(), GL_STATIC_DRAW );

		glVertexArrayElementBuffer( _vaoSkybox, _eboSkybox );

		return true;
	}


	bool LabWork7::_initShadingPass() {

		_uLightPosition = glGetUniformLocation( _shadingPassProgram, "uLight" );
		_uCameraPosition = glGetUniformLocation( _shadingPassProgram, "uCamera" );
		_uSpecularType	 = glGetUniformLocation( _shadingPassProgram, "uSpecularType" );
		_uVPShadowMappingShading = glGetUniformLocation( _shadingPassProgram, "uDepthVP" );

		_lightPosition = Vec3f( -7, 11, 2 );

		glProgramUniform3fv( _shadingPassProgram,
							 _uLightPosition,
							 1,
							 glm::value_ptr( _lightPosition ) );
		glProgramUniform3fv( _shadingPassProgram,
							 _uCameraPosition,
							 1,
							 glm::value_ptr( _camera.getPosition() ) );

		glProgramUniform1i( _shadingPassProgram, _uSpecularType, _specularType );



		_points.push_back( Vec2f( -1, -1 ) );
		_points.push_back( Vec2f( -1, 1 ) );
		_points.push_back( Vec2f( 1, 1 ) );
		_points.push_back( Vec2f( 1, -1 ) );

		_triangles.push_back( Vec3i( 0, 1, 2 ) );
		_triangles.push_back( Vec3i( 0, 2, 3 ) );

		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _points.size() * sizeof( Vec2f ), _points.data(), GL_STATIC_DRAW );

		// Create VAO
		glCreateVertexArrays( 1, &_vao );
			glEnableVertexArrayAttrib( _vao, 0 ); // Enable attribute at index 0

			glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 2 space between each object

			glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );

			glVertexArrayAttribBinding( _vao, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao

		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _triangles.size() * sizeof( Vec3i ), _triangles.data(), GL_STATIC_DRAW );

		glVertexArrayElementBuffer( _vao, _ebo );


		return true;
	}

	bool LabWork7::_initGBuffer() {

		_fboSize[ 0 ] = this->getWindowWidth();
		_fboSize[ 1 ] = this->getWindowHeight();
		
		glCreateFramebuffers( 1, &_fbo );

		glCreateTextures( GL_TEXTURE_2D, 6, _gBufferTextures );

		glTextureStorage2D( _gBufferTextures[ 0 ], 1, GL_RGB32F,  this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 1 ], 1, GL_RGB32F,  this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 2 ], 1, GL_RGB32F,  this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 3 ], 1, GL_RGB32F,  this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 4 ], 1, GL_RGBA32F, this->getWindowWidth(), this->getWindowHeight() );

		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32F,	  this->getWindowWidth(), this->getWindowHeight() );

		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _gBufferTextures[ 0 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT1, _gBufferTextures[ 1 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT2, _gBufferTextures[ 2 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT3, _gBufferTextures[ 3 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT4, _gBufferTextures[ 4 ], 0 );

		glNamedFramebufferTexture( _fbo, GL_DEPTH_ATTACHMENT , _gBufferTextures[ 5 ], 0 );
		
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
							 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

		glNamedFramebufferDrawBuffers( _fbo, 5, buffers );

		if ( glCheckNamedFramebufferStatus( _fbo, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "Error during Frame Buffer Object initialization" << std::endl;
			return false;
		}

		return true;
	}

	bool LabWork7::_initShaders()
	{
		const std::string fragShadingPassFile = readFile( _shaderFolder + "shading_pass.frag" );
		const std::string vertGeometryPassFile = readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragGeometryPassFile = readFile( _shaderFolder + "geometry_pass.frag" );
		const std::string vertShadowMapFile = readFile( _shaderFolder + "shadowmap.vert" );
		const std::string fragShadowMapFile = readFile( _shaderFolder + "shadowmap.frag" );

		const GLchar * fShadingPassSrc	= fragShadingPassFile.c_str();
		const GLchar * vGeometryPassSrc = vertGeometryPassFile.c_str();
		const GLchar * fGeometryPassSrc = fragGeometryPassFile.c_str();
		const GLchar * vShadowMapSrc	= vertShadowMapFile.c_str();
		const GLchar * fShadowMapSrc	= fragShadowMapFile.c_str();

		GLuint fragShadingPassShader  = glCreateShader( GL_FRAGMENT_SHADER );
		GLuint vertGeometryPassShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragGeometryPassShader = glCreateShader( GL_FRAGMENT_SHADER );
		GLuint vertShadowMapShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragShadowMapShader	  = glCreateShader( GL_FRAGMENT_SHADER );

		glShaderSource( fragShadingPassShader, 1, &fShadingPassSrc, NULL );
		glShaderSource( vertGeometryPassShader, 1, &vGeometryPassSrc, NULL );
		glShaderSource( fragGeometryPassShader, 1, &fGeometryPassSrc, NULL );
		glShaderSource( vertShadowMapShader, 1, &vShadowMapSrc, NULL );
		glShaderSource( fragShadowMapShader, 1, &fShadowMapSrc, NULL );

		glCompileShader( fragShadingPassShader );
		glCompileShader( vertGeometryPassShader );
		glCompileShader( fragGeometryPassShader );
		glCompileShader( vertShadowMapShader );
		glCompileShader( fragShadowMapShader );

		// Check for errors in compilation
		GLint compiled;
		glGetShaderiv( fragShadingPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShadingPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShadingPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( fragGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}
		glGetShaderiv( vertGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( fragShadowMapShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShadowMapShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShadowMapShader );
			glDeleteShader( vertShadowMapShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( vertShadowMapShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertShadowMapShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShadowMapShader );
			glDeleteShader( vertShadowMapShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		glAttachShader( _shadingPassProgram, fragShadingPassShader );
		glAttachShader( _geometryPassProgram, vertGeometryPassShader );
		glAttachShader( _geometryPassProgram, fragGeometryPassShader );
		glAttachShader( _shadowMappingProgram, vertShadowMapShader );
		glAttachShader( _shadowMappingProgram, fragShadowMapShader );

		glLinkProgram( _shadingPassProgram );
		// Check if link is ok
		GLint linked;
		glGetProgramiv( _shadingPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}
		glDeleteShader( fragShadingPassShader );

		glLinkProgram( _geometryPassProgram );
		// Check if link is ok
		glGetProgramiv( _geometryPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking geometry pass program : " << log << std ::endl;
			return false;
		}
		glDeleteShader( vertGeometryPassShader );
		glDeleteShader( fragGeometryPassShader );

		glLinkProgram( _shadowMappingProgram );
		// Check if link is ok
		glGetProgramiv( _shadowMappingProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadowMappingProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking geometry pass program : " << log << std ::endl;
			return false;
		}
		glDeleteShader( vertShadowMapShader );
		glDeleteShader( fragShadowMapShader );

	}

	bool LabWork7::_initShadowMapping() { 

		// Init ShadowMapping Program

			glUseProgram( _shadowMappingProgram );

			_uVPShadowMapping = glGetUniformLocation( _shadowMappingProgram, "uVPDepthMatrix" );
			_uMShadowMapping = glGetUniformLocation( _shadowMappingProgram, "uMDepthMatrix" );

			Vec3f lightInvDir = _lightPosition;

			// Compute the MVP matrix from the light's point of view
			Mat4f depthProjectionMatrix = glm::ortho<float>( -7, 10, -7, 7, -50, 70 );
			Mat4f depthViewMatrix		= glm::lookAt( lightInvDir, Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ) );
			Mat4f depthModelMatrix		= _mesh._transformation;
			Mat4f depthVP				= depthProjectionMatrix * depthViewMatrix;
			Mat4f depthMVP				= depthVP * depthModelMatrix;
			
			glProgramUniformMatrix4fv( _shadowMappingProgram, _uVPShadowMapping, 1, GL_FALSE, glm::value_ptr( depthVP ) );
			glProgramUniformMatrix4fv( _shadowMappingProgram, _uMShadowMapping, 1, GL_FALSE, glm::value_ptr( _mesh._transformation ) );
			glProgramUniformMatrix4fv( _shadingPassProgram, _uVPShadowMappingShading, 1, GL_FALSE, glm::value_ptr( depthVP ) );


		// Init FBO/Texture
			glCreateFramebuffers( 1, &_shadowMappingFBO );

			glCreateTextures( GL_TEXTURE_2D, 1, &_shadowMappingTexture );

			glTextureStorage2D( _shadowMappingTexture, 1, GL_DEPTH_COMPONENT16, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT );

			glNamedFramebufferTexture( _shadowMappingFBO, GL_DEPTH_ATTACHMENT, _shadowMappingTexture, 0 );

			glNamedFramebufferDrawBuffer( _shadowMappingFBO, GL_COLOR_ATTACHMENT0 );

			if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			{
				std::cerr << "Error during ShadowMap Frame Buffer Object initialization" << std::endl;
				return false;
			}
		
		return true;
	}


	void LabWork7::_updateShaders() { 

		glDeleteProgram( _shadingPassProgram );
		glDeleteProgram( _geometryPassProgram );
		glDeleteProgram( _shadowMappingProgram );

		_shadingPassProgram	 = glCreateProgram();
		_geometryPassProgram = glCreateProgram();
		_shadowMappingProgram = glCreateProgram();

		_initShaders();

		glProgramUniform3fv( _geometryPassProgram, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform1i ( _geometryPassProgram, _uSpecularType, _specularType );


	}

	void LabWork7::animate( const float p_deltaTime ) {
		

		// Handle camera movements
		float multiplier = capslock_pr ? 3 : 1;
		multiplier		 = lctrl_pr ? 0.2 : multiplier;
		if ( w_pr )
			_camera.moveFront( _cameraSpeed * multiplier * p_deltaTime );
		if ( s_pr )
			_camera.moveFront( -_cameraSpeed * multiplier * p_deltaTime );
		if ( a_pr )
			_camera.moveRight( -_cameraSpeed * multiplier * p_deltaTime );
		if ( d_pr )
			_camera.moveRight( _cameraSpeed * multiplier * p_deltaTime );
		if ( sp_pr )
			_camera.moveUp( _cameraSpeed * multiplier * p_deltaTime );
		if ( lsh_pr )
			_camera.moveUp( -_cameraSpeed * multiplier * p_deltaTime );

		if (_updateFov) {
			_camera.setFovy( _fov );
		}

	}

	void LabWork7::render() {

		_shadowPass();

		_geometryPass();

		if (_renderGBuffer) {
			glNamedFramebufferReadBuffer( _fbo, _bufferTypes[ _selectedBuffer ] );
			glBlitNamedFramebuffer( _fbo,
								0,
								0, 0,
								getWindowWidth(), getWindowHeight(),
								0,0,
								getWindowWidth(), getWindowHeight(),
								GL_COLOR_BUFFER_BIT,
								GL_NEAREST );

		} else {

			_shadingPass();

		}


	}

	void LabWork7::_shadowPass() {
		glUseProgram( _shadowMappingProgram );
		glEnable( GL_DEPTH_TEST );
		glViewport( 0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _shadowMappingFBO );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Compute the MVP matrix from the light's point of view

		// glm::perspective( glm::radians( _fovy ), _aspectRatio, _zNear, _zFar );
		Mat4f depthProjectionMatrix = glm::ortho<float>( -7, 7, -7, 7, -50, 70 );
		//Mat4f depthProjectionMatrix = glm::perspective( glm::radians( 60.f ), (float)SHADOWMAP_WIDTH / SHADOWMAP_HEIGHT, 0.1f, 50.f );
		Mat4f depthViewMatrix		= glm::lookAt( _lightPosition, _lightLookAt, Vec3f( 0, 1, 0 ) );
		Mat4f depthModelMatrix		= _mesh._transformation;
		Mat4f depthVP				= depthProjectionMatrix * depthViewMatrix;
		
		glProgramUniformMatrix4fv( _shadowMappingProgram, _uVPShadowMapping, 1, GL_FALSE, glm::value_ptr( depthVP ) );
		glProgramUniformMatrix4fv(
			_shadowMappingProgram, _uMShadowMapping, 1, GL_FALSE, glm::value_ptr( _mesh._transformation ) );
		glProgramUniformMatrix4fv(
			_shadingPassProgram, _uVPShadowMappingShading, 1, GL_FALSE, glm::value_ptr( depthVP ) );

		glCullFace( GL_FRONT );
		_mesh.renderShadow( _shadowMappingProgram );
		glCullFace( GL_BACK );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	}

	void LabWork7::_geometryPass() {

		glEnable( GL_DEPTH_TEST );
		glViewport( 0, 0, _fboSize[ 0 ], _fboSize[ 1 ] );
		
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fbo );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		Mat4f V				= _camera.getViewMatrix();
		Mat4f MVP			= _camera.getProjectionMatrix() * V * _mesh._transformation;
		Mat4f normalMatrix	= glm::transpose( glm::inverse( _mesh._transformation ) );
		
		glProgramUniformMatrix4fv( _geometryPassProgram, _uM,			 1, false, glm::value_ptr( _mesh._transformation ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, _uMVP,			 1, false, glm::value_ptr( MVP ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, _uNormalMatrix, 1, false, glm::value_ptr( normalMatrix ) );

		_mesh.render( _geometryPassProgram );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glBindVertexArray( 0 );

	}

	void LabWork7::_shadingPass() { 

		glUseProgram( _shadingPassProgram );
		glViewport( 0, 0, _fboSize[ 0 ], _fboSize[ 1 ] );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		glBindVertexArray( _vao );

		glDisable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		glBindTextureUnit( 0, _gBufferTextures[ 0 ] );
		glBindTextureUnit( 1, _gBufferTextures[ 1 ] );
		glBindTextureUnit( 2, _gBufferTextures[ 2 ] );
		glBindTextureUnit( 3, _gBufferTextures[ 3 ] );
		glBindTextureUnit( 4, _gBufferTextures[ 4 ] );

		glBindTextureUnit( 5, _shadowMappingTexture );

		glProgramUniform3fv( _shadingPassProgram,
							 _uLightPosition,
							 1, glm::value_ptr( _lightPosition ) );

		glProgramUniform3fv( _shadingPassProgram,
							 _uCameraPosition,
							 1,
							 glm::value_ptr( _camera.getPosition() ) );

		
		glDrawElements( GL_TRIANGLES, (GLsizei)( 3 * _triangles.size() ), GL_UNSIGNED_INT, 0 );

	}

	void LabWork7::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN || p_event.type == SDL_KEYUP )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				w_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_S: // Back
				s_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_A: // Left
				a_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_D: // Right
				d_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_SPACE: // Up
				sp_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_LSHIFT: // Bottom
				lsh_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_LCTRL: // Bottom
				lctrl_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_CAPSLOCK:
				capslock_pr = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			default: break;
			}
		}

		if ( p_event.type == SDL_MOUSEBUTTONDOWN )
		{
			switch ( p_event.button.button )
			{
			case SDL_BUTTON_LEFT:

				// Rotate the camera only when not on imgui
				if (!ImGui::GetIO().WantCaptureMouse) {
					rotate_camera = true;
					SDL_SetRelativeMouseMode( SDL_TRUE );
				}
				break;
			
			case SDL_BUTTON_RIGHT:
				rotate_camera = false;
				SDL_SetRelativeMouseMode( SDL_FALSE );

				int w, h;
				SDL_GetWindowSize( SDL_GetMouseFocus(), &w, &h );
				SDL_WarpMouseInWindow( NULL, w / 2, h / 2 );
				break;

			default: 
				break;
			}
		}

		if ( p_event.type == SDL_MOUSEBUTTONUP ) {
			switch ( p_event.button.button ) {

			default: 
				break;
			}
			
		}

		// Rotate when left click + motion
		if ( rotate_camera && p_event.type == SDL_MOUSEMOTION )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
		}
	}

	void LabWork7::displayUI()
	{
		ImGui::Begin( "Settings lab work 6" );
		
		#pragma region FOV Slider
			_updateFov = ImGui::SliderFloat( "FOV", &_fov, 30, 120, "%.0f", 1 );
		#pragma endregion

		#pragma region Light position selector
		if ( ImGui::DragFloat3( "Light position", glm::value_ptr( _lightPosition ), 0.05, -10, 10, "%.2f", 1 ) )
		{
			glProgramUniform3fv( _shadingPassProgram, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}
		#pragma endregion

		#pragma region Camera position selector
		Vec3f camPos = _camera.getPosition();
		if ( ImGui::DragFloat3( "Camera position", glm::value_ptr( camPos ), 0.05, -10.f, 10.f, "%.2f", 1 ) )
		{
			_camera.setPosition( camPos );
		}
		#pragma endregion

		#pragma region Set light position
		if ( ImGui::Button( "Set light at camera position" ) )
		{
			_lightPosition = _camera.getPosition();
			_lightLookAt = _camera.getLookAt();
			glProgramUniform3fv( _shadingPassProgram, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}
		#pragma endregion

		#pragma region Specular types selector
		if ( ImGui::RadioButton( "Blinn-Phong", _specularType == 2 ) )
		{
			_specularType = 2;
			glProgramUniform1i( _shadingPassProgram, _uSpecularType, _specularType );
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Phong", _specularType == 1 ) )
		{
			_specularType = 1;
			glProgramUniform1i( _shadingPassProgram, _uSpecularType, _specularType );
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "None", _specularType == 0 ) )
		{
			_specularType = 0;
			glProgramUniform1i( _shadingPassProgram, _uSpecularType, _specularType );
		}
		#pragma endregion

		#pragma region Shader recompiling button
		if ( ImGui::Button( "Recompile shaders" ) )
		{
			_updateShaders();
		}
		#pragma endregion

		#pragma region Rendered buffer selector

		if (ImGui::RadioButton("Render only one frame buffer", _renderGBuffer)) {
			_renderGBuffer = !_renderGBuffer;
		}
		if ( _renderGBuffer ) {
			std::string text = "Rendered buffer \n( ";
			text.append( _bufferTypesNames[ _selectedBuffer ] );
			text.append( " )" );

			ImGui::ListBox( text.c_str(), &_selectedBuffer, _bufferTypesNames, 6 );
		}

		
		
		#pragma endregion


		



		ImGui::End();
	}

	void LabWork7::_initCamera() { 
		_camera.setPosition( Vec3f( -2.5, 0.75, 0 ) );
		_camera.setLookAt( Vec3f( 0, 0.75, 0 ) );
		_camera.setFovy( 60 );
		_camera.print();
	}

	void LabWork7::resize( int p_width, int p_height )
	{ 
		
		if ( p_width != _fboSize[ 0 ] || p_height != _fboSize[ 1 ] ) {
			_windowWidth  = p_width;
			_windowHeight = p_height;
			glViewport( 0, 0, p_width, p_height );

			_initGBuffer();
		}

	}

	GLuint LabWork7::loadCubemap( std::vector<std::string> faces ) { 
		GLuint textureId;
		glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &textureId );

		// https://learnopengl.com/Advanced-OpenGL/Cubemaps
		int width, height, nrChannels;
		for ( unsigned int i = 0; i < faces.size(); i++ )
		{
			unsigned char * data = stbi_load( faces[ i ].c_str(), &width, &height, &nrChannels, 0 );
			if ( data )
			{
				glTextureSubImage2D( textureId, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );

				stbi_image_free( data );
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[ i ] << std::endl;
				stbi_image_free( data );
			}
		}

		return textureId;

	}

} // namespace M3D_ISICG
