#include "lab_work_4.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

namespace M3D_ISICG
{

	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4() {

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _program );

	}

	bool LabWork4::init()
	{
		std::cout << "Initializing lab work 4..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		//_mesh.load( "Bunny", "./data/models/bunny.obj" );
		 
		_mesh.load( "Conference", "./data/models/conference.obj" );
		_mesh._transformation = glm::scale( _mesh._transformation, Vec3f(0.003, 0.003, 0.003) );

		if (!_initShaders()) {
			return false;
		}

		glEnable( GL_DEPTH_TEST );

		_initCamera();

		glUseProgram( _program );

		
		_uMVP		   = glGetUniformLocation( _program, "uMVPMatrix" );
		_uMV		   = glGetUniformLocation( _program, "uMVMatrix" );
		_uV			   = glGetUniformLocation( _program, "uVMatrix" );
		_uNormalMatrix = glGetUniformLocation( _program, "uNormalMatrix" );

		_uLightPosition = glGetUniformLocation( _program, "uLightPosition" );
		_ufBlinn = glGetUniformLocation( _program, "ufBlinn" );
		_lightPosition	= Vec3f( 1, 1, 1 );

		
		glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform1i( _program, _ufBlinn, _blinnPhong );

		std::cout << "Done!" << std::endl;
		return true;
	}

	bool LabWork4::_initShaders() {
		const std::string vertShaderFile = readFile( _shaderFolder + "mesh.vert" );
		const std::string fragShaderFile = readFile( _shaderFolder + "mesh.frag" );

		const GLchar * vSrc = vertShaderFile.c_str();
		const GLchar * fSrc = fragShaderFile.c_str();

		_vertShader = glCreateShader( GL_VERTEX_SHADER );
		_fragShader = glCreateShader( GL_FRAGMENT_SHADER );

		glShaderSource( _vertShader, 1, &vSrc, NULL );
		glShaderSource( _fragShader, 1, &fSrc, NULL );

		glCompileShader( _vertShader );
		glCompileShader( _fragShader );

		// Check for errors in compilation
		GLint compiled;
		glGetShaderiv( _vertShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( _vertShader, sizeof( log ), NULL, log );
			glDeleteShader( _vertShader );
			glDeleteShader( _fragShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}
		glGetShaderiv( _fragShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( _fragShader, sizeof( log ), NULL, log );
			glDeleteShader( _fragShader );
			glDeleteShader( _vertShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		_program = glCreateProgram();

		glAttachShader( _program, _vertShader );
		glAttachShader( _program, _fragShader );

		glLinkProgram( _program );
		// Check if link is ok
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		

	}

	void LabWork4::_updateShaders() { 
		glDetachShader( _program, _vertShader );
		glDetachShader( _program, _fragShader );

		glDeleteShader( _vertShader );
		glDeleteShader( _fragShader );

		_initShaders();

		glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform1i( _program, _ufBlinn, _blinnPhong );


	}

	void LabWork4::animate( const float p_deltaTime ) {
		

		// Handle camera movements
		if ( w_pr )
			_camera.moveFront( _cameraSpeed * p_deltaTime );
		if ( s_pr )
			_camera.moveFront( -_cameraSpeed * p_deltaTime );
		if ( a_pr )
			_camera.moveRight( -_cameraSpeed * p_deltaTime );
		if ( d_pr )
			_camera.moveRight( _cameraSpeed * p_deltaTime );
		if ( sp_pr )
			_camera.moveUp( _cameraSpeed * p_deltaTime );
		if ( lsh_pr )
			_camera.moveUp( -_cameraSpeed * p_deltaTime );

		if (_updateFov) {
			_camera.setFovy( _fov );
		}


		if ( _setLightPos )
		{
		}

	}

	void LabWork4::render() {

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		
		
		_V = _camera.getViewMatrix();
		_MV	= _camera.getViewMatrix() * _mesh._transformation;
		_MVP = _camera.getProjectionMatrix() * _MV;
		_NormalMatrix = glm::transpose( glm::inverse( _MV ) );
		
		glProgramUniformMatrix4fv( _program, _uV, 1, false, glm::value_ptr( _V ) );
		glProgramUniformMatrix4fv( _program, _uMV, 1, false, glm::value_ptr( _MV ) );
		glProgramUniformMatrix4fv( _program, _uMVP, 1, false, glm::value_ptr( _MVP ) );
		glProgramUniformMatrix4fv( _program, _uNormalMatrix, 1, false, glm::value_ptr( _NormalMatrix ) );




		_mesh.render(_program);

		glBindVertexArray( 0 );

	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
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

			default: break;
			}
		}


		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
		}
	}

	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4" );

		_updateFov = ImGui::SliderFloat( "FOV", &_fov, 30, 120, "%.0f", 1 );
		
		if ( ImGui::DragFloat3( "Light position", glm::value_ptr( _lightPosition ), 0.05, -10, 10, "%.2f", 1 ) )
		{
			glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}
		Vec3f camPos = _camera.getPosition();
		if (ImGui::DragFloat3("Camera position", glm::value_ptr(camPos), 0.05, -10.f, 10.f, "%.2f", 1)) {
			_camera.setPosition( camPos );
		}

		if (ImGui::Button("Set light at camera position")) {
			_lightPosition = _camera.getPosition();
			glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}

		if (ImGui::RadioButton("Blinn-Phong", _blinnPhong == 2)) {
			_blinnPhong = 2;
			glProgramUniform1i( _program, _ufBlinn, _blinnPhong );
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Phong", _blinnPhong == 1 ) )
		{
			_blinnPhong = 1;
			glProgramUniform1i( _program, _ufBlinn, _blinnPhong );
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "None", _blinnPhong == 0 ) )
		{
			_blinnPhong = 0;
			glProgramUniform1i( _program, _ufBlinn, _blinnPhong );
		}

		if (ImGui::Button("Recompile shaders")) {
			_updateShaders();
		}

		ImGui::End();
	}

	void LabWork4::createPolygon( Vec2f center, int nb_edges, float radius ) { 
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

	void LabWork4::_initCamera() { 
		_camera.setPosition( Vec3f( 0, 1, 3 ) );
		_camera.setLookAt( Vec3f( 0, 0, 0 ) );
		_camera.setFovy( 60 );
		_camera.print();
	}
} // namespace M3D_ISICG
