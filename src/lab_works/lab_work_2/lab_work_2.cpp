#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>

namespace M3D_ISICG
{

	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2() {

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _program );

	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		
		glDisable( GL_DEPTH_TEST );

		createPolygon( Vec2f( 0.3, -0.2 ), 200, 0.5 );

		const std::string vertShaderFile = readFile( _shaderFolder + "lw2.vert" );
		const std::string fragShaderFile = readFile( _shaderFolder + "lw2.frag" );

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


		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _points.size() * sizeof( Vec2f ), _points.data(), GL_STATIC_DRAW );

		glCreateBuffers( 1, &_vboColor );
		glNamedBufferData( _vboColor, _pointColors.size() * sizeof( Vec3f ), _pointColors.data(), GL_STATIC_DRAW );

		// Create VAO
		glCreateVertexArrays( 1, &_vao ); 
			glEnableVertexArrayAttrib( _vao, 0 ); // Enable attribute at index 0
			glEnableVertexArrayAttrib( _vao, 1 );

			glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 2 space between each object
			glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

			glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
			glVertexArrayVertexBuffer( _vao, 1, _vboColor, 0, sizeof( Vec3f ) );

			glVertexArrayAttribBinding( _vao, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao 
			glVertexArrayAttribBinding( _vao, 1, 1 );

		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _triangles.size() * sizeof( Vec3i ), _triangles.data(), GL_STATIC_DRAW );

		glVertexArrayElementBuffer( _vao, _ebo );

		glUseProgram( _program );

		_uTranslationX = glGetUniformLocation( _program, "uTranslationX" );
		_uLuminosity	   = glGetUniformLocation( _program, "uOpacity" );

		_time = 0.f;
		_luminosity = 0.5f;
		
		glProgramUniform1f( _program, _uLuminosity, _luminosity );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {

		_time += p_deltaTime;
	
		glProgramUniform1f( _program, _uTranslationX, glm::sin( _time ) / 2 );
		if ( _updateLuminosity ) {
			glProgramUniform1f( _program, _uLuminosity, _luminosity );
		}
		if ( _updateBgColor ) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}
	
	}

	void LabWork2::render() {
	
		glClear( GL_COLOR_BUFFER_BIT );

		glBindVertexArray( _vao );

		glDrawElements( GL_TRIANGLES, (GLsizei)(3 * _triangles.size()), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		_updateLuminosity = ImGui::SliderFloat( "Luminosité", &_luminosity, 0.f, 1.f );
		_updateBgColor = ImGui::ColorEdit3( "Couleur de fond", &_bgColor.x);
		ImGui::End();
	}

	void LabWork2::createPolygon( Vec2f center, int nb_edges, float radius ) { 
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

} // namespace M3D_ISICG
