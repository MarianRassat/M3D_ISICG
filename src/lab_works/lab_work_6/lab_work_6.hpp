#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{

	class LabWork6 : public BaseLabWork
	{


	  public:
		LabWork6() : BaseLabWork() {}
		~LabWork6();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void resize( int w, int h ) override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;



	  private:

		bool _initGeometryPass();
		bool _initShadingPass();
		void _initCamera();
		bool _initShaders();
		bool _initGBuffer();
		void _updateShaders();

		void _geometryPass();
		void _shadingPass();

		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint _geometryPassProgram;
		GLuint _shadingPassProgram;

		GLuint _vbo;
		GLuint _vao;
		GLuint _ebo;

		GLuint _fbo;
		int	   _fboSize[ 2 ];

		Camera _camera;
		float  _fov = 60;
		bool   _updateFov;
		bool   _updateLightPosition;
		bool   _setLightPos;

		int	   _specularType = 2;
		GLuint _uSpecularType;
		
		float  _cameraSpeed = 1;
		float  _cameraSensitivity = 0.1;
		
		GLuint _uMVP; GLuint _uMV; GLuint _uNormalMatrix; GLuint _uLightPosition; GLuint _uV;
		Mat4f  _MVP;  Mat4f  _MV;  Mat4f  _NormalMatrix;  Vec3f  _lightPosition;  Mat4f  _V;
		

		TriangleMeshModel _mesh;

		float _time;

		std::vector<Vec2f> _points;
		std::vector<Vec3i> _triangles;

		bool w_pr;
		bool a_pr;
		bool s_pr;
		bool d_pr;
		bool sp_pr;
		bool lsh_pr;
		bool lctrl_pr;
		bool capslock_pr;

		bool rotate_camera;

		bool		 _renderGBuffer	 = false;
		int _selectedBuffer = 0;
		const char * _bufferTypesNames[ 6 ]
			= { "Position", "Normals", "Ambient", "Diffuse", "Specular", "Depth" };
		GLenum		 _bufferTypes[ 6 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_DEPTH_ATTACHMENT };
		GLuint		 _gBufferTextures[ 6 ]; // 0-Position 1-Normal 2-Ambient 3-Diffuse 4-Specular+shininess 5-Depth 

		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
