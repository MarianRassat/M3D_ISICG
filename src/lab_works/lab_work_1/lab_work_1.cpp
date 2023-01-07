#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{

	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1() {

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _program );

	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;

		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glDisable( GL_DEPTH_TEST ); // disable depth test to draw the triangle
		_program = glCreateProgram();

		_triangle = { 
			Vec2f( -0.5,  0.5 ), 
			Vec2f(  0.5,  0.5 ),
			Vec2f(  0.5, -0.5 ) 
		};

		// compile the shaders
			const std::string vertShaderFile = readFile( _shaderFolder + "lw1.vert" );
			const std::string fragShaderFile = readFile( _shaderFolder + "lw1.frag" );

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

		// link shaders to the program

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

		// delete shaders afterwards
			glDeleteShader( vertShader );
			glDeleteShader( fragShader );

		// create vao and vbo for the triangle
			glCreateBuffers( 1, &_vbo );
			glNamedBufferData( _vbo, _triangle.size() * sizeof( Vec2f ), _triangle.data(), GL_STATIC_DRAW );

			glCreateVertexArrays( 1, &_vao );
			glEnableVertexArrayAttrib( _vao, 0 );
			glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
			glVertexArrayAttribBinding( _vao, 0, 0 );

		glUseProgram( _program ); // only this program will ever be used, so it's fine to call this function in init

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() {
		
		// clear the previous screen's color buffers
		glClear( GL_COLOR_BUFFER_BIT );

		// say we'll use this VAO
		glBindVertexArray( _vao );

		// draw the triangle described in the bound VAO
		glDrawArrays( GL_TRIANGLES, 0, (GLsizei)_triangle.size() );

		// unbind the VAO (useless here as there aren't other calls to draw VAOs)
		glBindVertexArray( 0 );

	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
