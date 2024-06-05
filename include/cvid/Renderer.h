#pragma once
#include <cvid/Vector.h>
#include <cvid/Window.h>
#include <vector>

namespace cvid
{
	struct Vertice
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 color;
	};

	//Render an amount of vertices to the window's framebuffer
	void DrawVertices(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices);
	//Render an amount of vertices as wireframes to the window's framebuffer
	void DrawVerticesWireframe(Window* window, std::vector<Vertice> vertices, std::vector<Vector3Int> indices);
}