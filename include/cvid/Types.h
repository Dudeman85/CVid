#pragma once
#include <array>
#include <cvid/Vector.h>

namespace cvid
{
	struct Sphere
	{
		//Center point in world space
		Vector3 center;
		Vector3 farthestPoint;
		float radius = 0;
	};

	struct Tri
	{
		Vector3 v0;
		Vector3 v1;
		Vector3 v2;
	};
	struct Tri2D
	{
		Vector2 v0;
		Vector2 v1;
		Vector2 v2;
	};

	struct Vertex
	{
		Vector3 position;
		Vector3 color;
	};
	//8-bit RGBA color
	struct Color
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 255;
	};

	//A face (triangle) that uses it's own texture coords and vertices
	struct Face
	{
		Tri vertices;
		Tri2D texCoords;
	};

	//A face (triangle) that used indexed texture coords and vertices
	struct IndexedFace
	{
		std::array<uint32_t, 3> verticeIndices = {};
		std::array<uint32_t, 3> texCoordIndices = {};
	};
}