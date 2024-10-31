#pragma once
#include <string>
#include <vector>
#include <array>
#include <cvid/Vector.h>
#include <cvid/Helpers.h>

namespace cvid
{
	struct Vertex
	{
		Vector3 position;
		Vector3 color;
	};

	//A face always has 3 vertices (triangle)
	struct Face 
	{
		std::array<uint32_t, 3> verticeIndices = {};
		std::array<uint32_t, 3> texCoordIndices = {};
		//Surface normal (calculated)
		Vector3 normal;

		//Temporary for testing
		cvid::Color color;
	};

	class Model
	{
	public:
		Model(std::string path);

		//Every face in the model, contains indices to vertices and texCoords vectors
		std::vector<Face> faces;
		//Vertices are shared for the whole model
		std::vector<Vertex> vertices;
		//Texture coordinates are shared for the whole model
		std::vector<Vector2> texCoords;
	};
}