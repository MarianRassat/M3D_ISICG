#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{

	struct Mesh
	{
		std::vector<Vec3f> points;
		std::vector<Vec3f> colors;
		std::vector<unsigned int> triangles;
		Mat4f transformation;
		
		GLuint vbo;
		GLuint vboColor;
		GLuint vao;
		GLuint ebo;
	};

	class LabWork3 : public BaseLabWork
	{


	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void createPolygon( Vec2f center, int nb_edges, float radius );
		void createCube();


	  private:

		void _updateViewMatrix();
		void _updateProjMatrix();
		void _initCamera();

		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint _program;
		GLuint _vbo;
		GLuint _vboColor;
		GLuint _vao;
		GLuint _ebo;

		Camera _camera;
		float  _fov = 60;
		bool _updateFov;
		float  _cameraSpeed = 0.1;
		float  _cameraSensitivity = 0.1;

		GLuint _uTransformMatrix;
		GLuint _uViewMatrix;
		GLuint _uProjMatrix;

		Mesh _cube;

		float _time;

		std::vector<Vec2f> _points;
		std::vector<Vec3f> _pointColors;
		std::vector<Vec3i> _triangles;


		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
