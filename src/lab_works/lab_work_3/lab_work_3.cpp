#include "lab_work_3.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace M3D_ISICG
{

	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() {

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _program );

	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string vertShaderFile = readFile( _shaderFolder + "lw3.vert" );
		const std::string fragShaderFile = readFile( _shaderFolder + "lw3.frag" );

		const GLchar * vSrc = vertShaderFile.c_str();
		const GLchar * fSrc = fragShaderFile.c_str();

		GLuint vertShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragShader = glCreateShader( GL_FRAGMENT_SHADER );

		glShaderSource( vertShader, 1, &vSrc, NULL );
		glShaderSource( fragShader, 1, &fSrc, NULL );

		glCompileShader( vertShader );
		glCompileShader( fragShader );


		// Check for errors in compilation
		GLint compiled;
		glGetShaderiv( vertShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertShader, sizeof( log ), NULL, log );
			glDeleteShader( vertShader );
			glDeleteShader( fragShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}
		glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShader );
			glDeleteShader( vertShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}


		_program = glCreateProgram();

		glAttachShader( _program, vertShader );
		glAttachShader( _program, fragShader );

		glLinkProgram( _program );
		// Check if link is ok
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar						  log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( vertShader );
		glDeleteShader( fragShader );

		glEnable( GL_DEPTH_TEST );

		createCube();

		_initCamera();

		_uViewMatrix = glGetUniformLocation( _program, "uViewMatrix" );
		_uProjMatrix = glGetUniformLocation( _program, "uProjMatrix" );

		glUseProgram( _program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( const float p_deltaTime ) {

		_cube.transformation = glm::rotate( _cube.transformation, p_deltaTime, Vec3f( 0.f, 1.f, 1.f ) );

		if (_updateFov) {
			_camera.setFovy( _fov );
		}

		glProgramUniformMatrix4fv( _program, _uTransformMatrix, 1, false, glm::value_ptr( _cube.transformation ) );

	}

	void LabWork3::render() {

		_updateViewMatrix();
		_updateProjMatrix();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glBindVertexArray( _cube.vao );

		glDrawElements( GL_TRIANGLES, (GLsizei)(_cube.triangles.size()), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );

	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		_updateFov = ImGui::SliderFloat( "FOV", &_fov, 30, 120, "%.0f", 1 );
		ImGui::End();
	}

	void LabWork3::createPolygon( Vec2f center, int nb_edges, float radius ) { 
		_points.push_back( Vec2f(center) );
		_pointColors.push_back( getRandomVec3f() );

		if (nb_edges < 3) {
			nb_edges = 3;
		}

		_points.push_back( Vec2f(1, 0) * radius + center);
		_pointColors.push_back( getRandomVec3f() );

		for (int i = 1; i < nb_edges; i++) {

			_points.push_back( 
				Vec2f(	
					glm::cos( 2*glm::pi<float>() / nb_edges * (float)i ),
					glm::sin( 2*glm::pi<float>() / nb_edges * (float)i ) 
				) * radius + center
			);

			_pointColors.push_back( getRandomVec3f() );

			_triangles.push_back( Vec3i( i, 0, i+1 ) );

		}

		_triangles.push_back( Vec3i( nb_edges, 0, 1 ) );

	}

	void LabWork3::createCube()
	{ 

		for ( float x = -0.5; x <= 0.5f; x += 1.f ) {
			for ( float y = -0.5; y <= 0.5f; y += 1.f ) {
				for ( float z = -0.5; z <= 0.5f; z += 1.f ) {
					
					_cube.points.push_back( Vec3f( x, y, z ) );

					std::cout << x << " " << y << " " << z << std::endl;

					_cube.colors.push_back( getRandomVec3f() );
				
				}
			}
		}

		_cube.triangles = { 
				0, 1, 4,
				5, 1, 4,
				0, 2, 1,
				3, 2, 1,
				4, 5, 6,
				7, 5, 6,
				0, 4, 2,
				6, 4, 2,
				2, 3, 6,
				7, 3, 6,
				1, 3, 5,
				7, 3, 5
		};
		_cube.transformation = glm::identity<Mat4f>();
		_cube.transformation = glm::scale( _cube.transformation, glm::vec3( 0.8f ) );

		_uTransformMatrix = glGetUniformLocation( _program, "uTransformMatrix" );
		
		glProgramUniformMatrix4fv( _program, _uTransformMatrix, 1, false, glm::value_ptr( _cube.transformation ) );

		glCreateBuffers( 1, &_cube.vbo );
		glNamedBufferData( _cube.vbo, _cube.points.size() * sizeof( Vec3f ), _cube.points.data(), GL_STATIC_DRAW );

		glCreateBuffers( 1, &_cube.vboColor );
		glNamedBufferData( _cube.vboColor, _cube.colors.size() * sizeof( Vec3f ), _cube.colors.data(), GL_STATIC_DRAW );

		
		// Create VAO
		glCreateVertexArrays( 1, &_cube.vao );
			glEnableVertexArrayAttrib( _cube.vao, 0 ); // Enable attribute at index 0
			glEnableVertexArrayAttrib( _cube.vao, 1 );

			glVertexArrayAttribFormat( _cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 3 space between each object
			glVertexArrayAttribFormat( _cube.vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

			glVertexArrayVertexBuffer( _cube.vao, 0, _cube.vbo, 0, sizeof( Vec3f ) );
			glVertexArrayVertexBuffer( _cube.vao, 1, _cube.vboColor, 0, sizeof( Vec3f ) );

			glVertexArrayAttribBinding( _cube.vao, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao
			glVertexArrayAttribBinding( _cube.vao, 1, 1 );

		glCreateBuffers( 1, &_cube.ebo );
			glNamedBufferData( _cube.ebo, _cube.triangles.size() * sizeof( unsigned int ), _cube.triangles.data(), GL_STATIC_DRAW );

			glVertexArrayElementBuffer( _cube.vao, _cube.ebo );

	}

	void LabWork3::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uViewMatrix, 1, false, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uProjMatrix, 1, false, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork3::_initCamera() { 
		_camera.setPosition( Vec3f( 0, 1, 3 ) );
		_camera.setLookAt( Vec3f( 0, 0, 0 ) );
		_camera.setFovy( 60 );
		_camera.print();
	}
} // namespace M3D_ISICG
