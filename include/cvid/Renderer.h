#pragma once
#include <vector>
#include <cvid/Vector.h>
#include <cvid/Window.h>
#include <cvid/Matrix.h>
#include <cvid/Camera.h>
#include <cvid/Model.h>

namespace cvid
{
	//Render a point to the window's framebuffer
	void DrawPoint(Vector3 point, Color color, Matrix4 transform, Camera* cam, Window* window);
	//Render a line to the window's framebuffer
	void DrawLine(Vector3 p1, Vector3 p2, Color color, Matrix4 transform, Camera* cam, Window* window);
	//Render indexed filled triangles from vertices to the window's framebuffer
	void DrawVertices(std::vector<Vertex> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window);
	//Render indexed wireframe triangles from vertices to the window's framebuffer
	void DrawVerticesWireframe(std::vector<Vertex> vertices, std::vector<Vector3Int> indices, Matrix4 transform, Camera* cam, Window* window);
	//Render a model to the window's framebuffer
	void DrawModel(Model* model, Matrix4 transform, Camera* cam, Window* window);

	//Utility Functions
	//Returns true if a model falls entirely inside a camera's clip space
	bool ClipModel(const std::vector<Vertex>& modelVerts, Camera* cam);
	//Returns true if a tri falls entirely inside a camera's clip space
	bool ClipTri(const Vector3& v1, const Vector3& v2, const Vector3& v3, Camera* cam);
}