#pragma once
#include <vector>

namespace cvid 
{
	//Linearly interpolate a value between start and end range times, will include start and end in the result
	std::vector<float> LerpRange(int range, float start, float end);
}