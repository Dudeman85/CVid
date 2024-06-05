#pragma once
#include <vector>
#include <glm/matrix.hpp>
#include <cvid/Vector.h>
#include <cvid/Window.h>

namespace cvid
{
	struct Vertice
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 color;
	};

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, glm::mat4x4 mvp);
	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices, glm::mat4x4 mvp);
}