#pragma once
#include <string>
#include <vector>
#include <cvid/Vector.h>

namespace cvid
{
	struct Vertice
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 color;
	};

	class Model
	{
	public:
		Model(std::string path);

		std::vector<cvid::Vertice> vertices;
		std::vector<cvid::Vector3Int> indices;
	};
}