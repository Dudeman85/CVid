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
	inline Attributes AttribChangePerD(Attributes a, Attributes b, int d);
	//Add two attributes together
	inline Attributes AddAttribs(Attributes a, Attributes b);

	//Draw a point onto a window's framebuffer
	void RasterizePoint(Window* window, Vector3 pt, Color color);
	//Draw a line onto a window's framebuffer
	void RasterizeLine(Window* window, Vector3 v0, Vector3 v1, Color color);
	//Interpolate vertex attributes for each position between start and end (inclusive), left or right edge is prioritized
	std::vector<Attributes> InterpolateAttributes(Vector2Int start, Vector2Int end, Attributes a, Attributes b, bool prioritizeLeft = false);
	//Draw a triangle onto a window's framebuffer based on a material and attributes
	void RasterizeTriangle(Window* window, Face triangle, const Material* mat = nullptr);
	//Draw a triangle onto a window's framebuffer entirely of one color
	void RasterizeTriangle(Window* window, Tri verts, Color color);
	//Draw a triangle onto a window's framebuffer
	void RasterizeTriangleWireframe(Window* window, Tri verts, Color color);
		
	//The global directional light (only one supported for now)
	inline Vector3 directionalLight;
	inline double directionalLightIntensity = 0;
	//The ambient light in the scene from 0-1
	inline double ambientLightIntensity = 1;
}