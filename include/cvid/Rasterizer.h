#pragma once
#include <vector>
#include <cvid/Window.h>
#include <cvid/Vector.h>
#include <cvid/Model.h>
#include <cvid/Types.h>

namespace cvid
{
	//A struct to store vertex attributes
	struct Attributes
	{
		int x;
		double z;
		Vector2 texCoord;
	};
	//Difference between two attributes
	inline Attributes ChangePerD(Attributes a, Attributes b, int d)
	{
		return Attributes{ 0, (a.z - b.z) / d, (a.texCoord - b.texCoord) / d };
	}
	//Add two attributes together
	inline Attributes Add(Attributes a, Attributes b) 
	{
		return Attributes{ a.x + b.x, a.z + b.z, a.texCoord + b.texCoord };
	}

	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, Color color);
	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 v0, Vector3 v1, Color color);
	//Interpolate vertex attributes for each position between start and end (inclusive), left or right edge is prioritized
	std::vector<Attributes> InterpolateAttributes(Vector2Int start, Vector2Int end, Attributes a, Attributes b, bool prioritizeLeft = false);
	//Draw a triangle onto a window's framebuffer
	void RasterizeTriangle(Window* window, Face triangle, const Material* mat = nullptr);
	//Draw a triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Vector3 v0, Vector3 v1, Vector3 v2, Color color);
}