#pragma once
#include <string>
#include <vector>
#include <cvid/Types.h>
#include <cvid/Vector.h>

namespace cvid
{
	class Texture
	{
	public:
		//Load a texture from file
		Texture(std::string path);

		//Get the color of a pixel at position
		Color GetPixel(int x, int y);
		Color GetPixel(Vector2Int pos);

		//Name of the file
		std::string name;
		//Size in pixels
		int width = 0;
		int height = 0;
		//Pixel data, accessed [y * width + x] 
		std::vector<Color> data;

	private:
		Texture(const Texture& tex);
	};
}