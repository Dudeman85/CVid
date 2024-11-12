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
		Vector3 v1;
		Vector3 v2;
		Vector3 v3;
	};
	struct Tri2D
	{
		Vector2 v1;
		Vector2 v2;
		Vector2 v3;
	};

	struct Vertex
	{
		Vector3 position;
		Vector3 color;
	};

	//A face (triangle) that uses it's own texture coords and vertices
	struct Face
	{
		Tri vertices;
		Tri2D texCoords;
		//Surface normal (calculated)
		Vector3 normal;
		//Should this face be rendered
		bool culled = false;
	};

	//A face (triangle) that used indexed texture coords and vertices
	struct IndexedFace
	{
		std::array<uint32_t, 3> verticeIndices = {};
		std::array<uint32_t, 3> texCoordIndices = {};
		//Surface normal (calculated)
		Vector3 normal;
		//Should this face be rendered
		bool culled = false;
	};
}