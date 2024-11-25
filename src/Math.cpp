#include <cvid/Math.h>

namespace cvid
{
	//Linearly interpolate a value between start and end range times, will include start and end in the result
	std::vector<float> LerpRange(int range, float start, float end)
	{
		std::vector<float> values;
		values.reserve(range + 2);

		//Slope of the line
		float m = (end - start) / (range + 1);

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

	//Linearly interpolate a Vector2 between start and end range times, will include start and end in the result
	std::vector<Vector2> LerpRange2D(int range, Vector2 start, Vector2 end)
	{
		std::vector<Vector2> values;
		values.reserve(range + 2);

		//Slope
		Vector2 m = (end - start) / (range + 1);

		//For each position in range interpolate d
		Vector2 d = start;
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