#include <cvid/Math.h>

namespace cvid
{
	//Linearly interpolate a value between start and end range times, will include start and end in the result
	std::vector<float> LerpRange(int range, float start, float end)
	{
		std::vector<float> values;
		values.reserve(range);
		
		//Slope of the line
		float m = (end - start) / range;

		//For each position in range interpolate d
		float d = start;
		values.push_back(d);
		for (int i = 0; i < range; i++)
		{
			d += m;
			values.push_back(d);
		}
		values.push_back(end);

		return values;
	}
}