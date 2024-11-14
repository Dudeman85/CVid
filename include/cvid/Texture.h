#pragma once
#include <string>
#include <cvid/Vector.h>

namespace cvid
{
	class Texture
	{
	public:
		//Load a texture from file
		Texture(std::string path);

		std::string name;
		//Size in pixels
		Vector2Int size;
	};
}