#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cvid/Texture.h>
#include <cvid/Helpers.h>

namespace cvid
{
	//Load texture from file
	Texture::Texture(std::string path)
	{
		name = path;

		//Flip texture
		stbi_set_flip_vertically_on_load(true);

		int n;
		//Attempt to load the image
		unsigned char* rawData = stbi_load(path.c_str(), &width, &height, &n, 4);

		//Check for failures
		if (!rawData)
		{
			cvid::LogError("Failed to load texture from file " + path);
			return;
		}

		//Convert the pixel data to a more easily usable format
		data.reserve((size_t)width * height);
		for (size_t i = 0; i < (size_t)width * height * 4; i += 4)
		{
			data.push_back(Color{ rawData[i], rawData[i + 1], rawData[i + 2], rawData[i + 3] });
		}

		//Delete the raw pixel data
		stbi_image_free(rawData);
	}

	//Get the color of a pixel at position
	Color Texture::GetPixel(int x, int y)
	{
		//Check in bounds
		if (x >= width || x < 0 || y >= height || y < 0)
		{
			LogWarning("Texture position out of bounds!");
			return Color();
		}

		return data[(size_t)y * width + x];
	}
	//Get the color of a pixel at position
	Color Texture::GetPixel(Vector2Int pos)
	{
		return GetPixel(pos.x, pos.y);
	}
}