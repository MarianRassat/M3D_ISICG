#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		bool _initShaders();
		bool _initVAO();
		void createPolygon( Vec2f center, int nb_edges, float radius );

	  private:
		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint _program;
		GLuint _vbo;
		GLuint _vboColor;
		GLuint _vao;
		GLuint _ebo;

		GLint _uTranslationX;
		GLint _uLuminosity;

		float _time;
		float _luminosity;

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

#endif // __LAB_WORK_2_HPP__
