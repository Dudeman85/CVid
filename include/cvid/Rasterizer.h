#pragma once
#include <vector>
#include <cvid/Window.h>
#include <cvid/Vector.h>

namespace cvid
{
	//Draw a point onto a window's framebuffer
	void DrawPoint(Window* window, Vector2 p1, Color color);
	//Draw a line onto a window's framebuffer
	void DrawLine(Window* window, Vector2Int p1, Vector2Int p2, Color color);
	//Get the x coordinates of a line
	std::vector<int> InterpolateX(Vector2Int p1, Vector2Int p2);
	//Draw a triangle onto a window's framebuffer
	void DrawTriangle(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color);
	//Draw a triangle onto a window's framebuffer
	void DrawTriangleWireframe(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color);
}