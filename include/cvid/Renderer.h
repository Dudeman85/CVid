#pragma once
#include <vector>
#include <glm/matrix.hpp>
#include <cvid/Vector.h>
#include <cvid/Window.h>
#include <cvid/Matrix.h>
#include <cvid/Camera.h>
#include <cvid/Model.h>

namespace cvid
{
	//Render indexed filled triangles from vertices to the window's framebuffer
	void DrawVertices(std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window);
	//Render indexed wireframe triangles from vertices to the window's framebuffer
	void DrawVerticesWireframe(std::vector<Vertice> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window);
	//Render a model to the window's frambuffer
	inline void DrawModel(Model* model, Matrix4 transform, Camera* cam, Window* window);
	//Render a model as wireframe to the window's frambuffer
	inline void DrawModelWireframe(Model* model, Matrix4 transform, Camera* cam, Window* window);
}