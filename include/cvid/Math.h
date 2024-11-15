#pragma once
#include <vector>
#include <cvid/Vector.h>

namespace cvid
{
	//Linearly interpolate a value between start and end range times, will include start and end in the result
	std::vector<float> LerpRange(int range, float start, float end);
	//Linearly interpolate a Vector2 between start and end range times, will include start and end in the result
	std::vector<Vector2> LerpRange2D(int range, Vector2 start, Vector2 end);
}