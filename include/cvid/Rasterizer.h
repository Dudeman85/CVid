#pragma once
#include <vector>
#include <cvid/Window.h>
#include <cvid/Vector.h>
#include <cvid/Model.h>
#include <cvid/Types.h>

namespace cvid
{
	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, Vector3Int color);
	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 p1, Vector3 p2, Vector3Int color);
	//Get the x coordinates of a line
	std::vector<int> InterpolateX(Vector2Int p1, Vector2Int p2);
	//Draw a triangle onto a window's framebuffer
	void RasterizeTriangle(Window* window, Face triangle, const Material* mat = nullptr);
	//Draw a triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Vector3 p1, Vector3 p2, Vector3 p3, Vector3Int color);
}