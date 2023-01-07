#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

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
		
		// disable the depth test, we draw 2D triangles
		glDisable( GL_DEPTH_TEST );

		// create a polygon centered in 0.3, -0.2 with 200 points and a radius of 0.5
		createPolygon( Vec2f( 0.3, -0.2 ), 200, 0.5 );

		_program = glCreateProgram();

		if ( !_initShaders() )
			return false;

		if ( !_initVAO() )
			return false;

		glUseProgram( _program );

		// get the uniform locations
		_uTranslationX	= glGetUniformLocation( _program, "uTranslationX" );
		_uLuminosity	= glGetUniformLocation( _program, "uOpacity" );

		_time		= 0.f;

		_luminosity = 0.5f;
		
		// pass luminosity as it won't be passed every frame or animation update
		glProgramUniform1f( _program, _uLuminosity, _luminosity );

		std::cout << "Done!" << std::endl;
		return true;
	}

	bool LabWork2::_initShaders()
	{
		// compile the shaders
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

		// attach shaders
		glAttachShader( _program, vertShader );
		glAttachShader( _program, fragShader );

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

		// delete shaders
		glDeleteShader( vertShader );
		glDeleteShader( fragShader );

		return true;
	}

	bool LabWork2::_initVAO()
	{
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _points.size() * sizeof( Vec2f ), _points.data(), GL_STATIC_DRAW );

		glCreateBuffers( 1, &_vboColor );
		glNamedBufferData( _vboColor, _pointColors.size() * sizeof( Vec3f ), _pointColors.data(), GL_STATIC_DRAW );

		// Create VAO
		glCreateVertexArrays( 1, &_vao );
		glEnableVertexArrayAttrib( _vao, 0 ); // Enable attribute at index 0
		glEnableVertexArrayAttrib( _vao, 1 );

		glVertexArrayAttribFormat(
			_vao, 0, 2, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 2 space between each object
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
		glVertexArrayVertexBuffer( _vao, 1, _vboColor, 0, sizeof( Vec3f ) );

		glVertexArrayAttribBinding( _vao, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao
		glVertexArrayAttribBinding( _vao, 1, 1 );

		glCreateBuffers( 1, &_ebo );
		// set ebo data
		glNamedBufferData( _ebo, _triangles.size() * sizeof( Vec3i ), _triangles.data(), GL_STATIC_DRAW );

		glVertexArrayElementBuffer( _vao, _ebo );

		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {

		_time += p_deltaTime;
	
		glProgramUniform1f( _program, _uTranslationX, glm::sin( _time ) / 2 );
	
	}

	void LabWork2::render() {
		// clear the color
		glClear( GL_COLOR_BUFFER_BIT );

		// bind the vao to use
		glBindVertexArray( _vao );

		// draw the elements described in the vao's ebo
		glDrawElements( GL_TRIANGLES, (GLsizei)(3 * _triangles.size()), GL_UNSIGNED_INT, 0 );

		// unbind the vao (useless in this program)
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{

		ImGui::Begin( "Settings lab work 2" );

		// update the uniforms if the values are modified

		if (ImGui::SliderFloat("Luminosité", &_luminosity, 0.f, 1.f)) {
			glProgramUniform1f( _program, _uLuminosity, _luminosity );
		}

		if ( ImGui::ColorEdit3( "Couleur de fond", glm::value_ptr(_bgColor) ) ) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		ImGui::End();

	}

	void LabWork2::createPolygon( Vec2f center, int nb_edges, float radius ) { 
		_points.push_back( Vec2f(center) ); // _points[0] is the center
		_pointColors.push_back( getRandomVec3f() );

		if (nb_edges < 3) {
			nb_edges = 3;
		}

		_points.push_back( Vec2f(1, 0) * radius + center); 
		_pointColors.push_back( getRandomVec3f() );

		for (int i = 1; i < nb_edges; i++) {

			_points.push_back( // add every single other point
				Vec2f(	
					glm::cos( 2*glm::pi<float>() / nb_edges * (float)i ),
					glm::sin( 2*glm::pi<float>() / nb_edges * (float)i ) 
				) * radius + center
			);

			_pointColors.push_back( getRandomVec3f() );

			_triangles.push_back( Vec3i( i, 0, i+1 ) ); // add a triangle description

		}

		_triangles.push_back( Vec3i( nb_edges, 0, 1 ) ); // add the last triangle

	}

} // namespace M3D_ISICG
