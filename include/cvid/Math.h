#pragma once
#include <vector>
#include <cvid/Vector.h>

namespace cvid
{
	//Linearly interpolate values for each step between start and end (both inclusive)
	std::vector<float> LerpRange(int start, int end, float a, float b);
	//Linearly interpolate Vector2 values for each step between start and end (both inclusive)
	std::vector<Vector2> LerpRange2D(int start, int end, Vector2 a, Vector2 b);
}