#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );
		glBindVertexArray( _vao );

		GLint uAmbientColor = glGetUniformLocation( p_glProgram, "uAmbientColor" );
		glProgramUniform3fv( p_glProgram, uAmbientColor, 1, glm::value_ptr( _material._ambient ) );

		GLint uDiffuseColor = glGetUniformLocation( p_glProgram, "uDiffuseColor" );
		glProgramUniform3fv( p_glProgram, uDiffuseColor, 1, glm::value_ptr( _material._diffuse ) );

		GLint uHasAmbientMap = glGetUniformLocation( p_glProgram, "uHasAmbientMap" );
		GLint uHasDiffuseMap = glGetUniformLocation( p_glProgram, "uHasDiffuseMap" );
		GLint uHasSpecularMap = glGetUniformLocation( p_glProgram, "uHasSpecularMap" );
		GLint uHasShininessMap = glGetUniformLocation( p_glProgram, "uHasShininessMap" );
		GLint uHasNormalMap = glGetUniformLocation( p_glProgram, "uHasNormalMap" );
		if ( _material._hasAmbientMap  == 1 ) {
			glProgramUniform1i( p_glProgram, uHasAmbientMap, 0 );
			glBindTextureUnit( 0, _material._ambientMap._id );
		} else {
			glProgramUniform1i( p_glProgram, uHasAmbientMap, 0 );
		}
		if ( _material._hasDiffuseMap  == 1 ) {
			glProgramUniform1i( p_glProgram, uHasDiffuseMap, 1 );
			glBindTextureUnit( 1, _material._diffuseMap._id );
		} else {
			glProgramUniform1i( p_glProgram, uHasDiffuseMap, 0 );
		}
		if ( _material._hasSpecularMap == 1 ) {
			glProgramUniform1i( p_glProgram, uHasSpecularMap, 1 );
			glBindTextureUnit( 2, _material._specularMap._id );
		} else {
			glProgramUniform1i( p_glProgram, uHasSpecularMap, 0 );
		}
		if ( _material._hasShininessMap  == 1 ) {
			glProgramUniform1i( p_glProgram, uHasShininessMap, 1 );
			glBindTextureUnit( 3, _material._shininessMap._id );
		} else {
			glProgramUniform1i( p_glProgram, uHasShininessMap, 0 );
		}
		if ( _material._hasNormalMap  == 1 ) {
			glProgramUniform1i( p_glProgram, uHasNormalMap, 1 );
			glBindTextureUnit( 4, _material._normalMap._id );
		} else {
			glProgramUniform1i( p_glProgram, uHasNormalMap, 0 );
		}

		GLint uSpecularColor = glGetUniformLocation( p_glProgram, "uSpecularColor" );
		glProgramUniform3fv( p_glProgram, uSpecularColor, 1, glm::value_ptr( _material._specular ) );
		
		GLint uShininess = glGetUniformLocation( p_glProgram, "uShininess" );
		glProgramUniform1f( p_glProgram, uShininess, _material._shininess );
		

		glDrawElements( GL_TRIANGLES, (GLsizei)( _indices.size() ), GL_UNSIGNED_INT, 0 );

		glBindTextureUnit( 0, 0 );
	}

	void TriangleMesh::renderShadow( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );
		glBindVertexArray( _vao );

		glDrawElements( GL_TRIANGLES, (GLsizei)( _indices.size() ), GL_UNSIGNED_INT, 0 );

		glBindTextureUnit( 0, 0 );
	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );

		glDeleteVertexArrays( 1, &_vao );
		
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL() { 
		glCreateBuffers( 1, &_vbo );
			glNamedBufferData( _vbo, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );
		
		glCreateVertexArrays( 1, &_vao );
			glEnableVertexArrayAttrib( _vao, 0 );
			glEnableVertexArrayAttrib( _vao, 1 );
			glEnableVertexArrayAttrib( _vao, 2 );
			glEnableVertexArrayAttrib( _vao, 3 );
			glEnableVertexArrayAttrib( _vao, 4 );

			glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _position) );
			glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
			glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) );
			glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
			glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );

			glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

			glVertexArrayAttribBinding( _vao, 0, 0 );
			glVertexArrayAttribBinding( _vao, 1, 0 );
			glVertexArrayAttribBinding( _vao, 2, 0 );
			glVertexArrayAttribBinding( _vao, 3, 0 );
			glVertexArrayAttribBinding( _vao, 4, 0 );

		glCreateBuffers( 1, &_ebo );
			glNamedBufferData( _ebo, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );
			glVertexArrayElementBuffer( _vao, _ebo );
	}
} // namespace M3D_ISICG
