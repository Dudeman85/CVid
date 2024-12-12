#include <cvid/Math.h>
#include <cvid/Math.h>

namespace cvid
{
	//Linearly interpolate values for each point between start and end (both inclusive)
	std::vector<float> LerpRange(int start, int end, float a, float b)
	{
		int range = abs(start - end);
		std::vector<float> values;
		values.reserve(range);

		//Slope
		float m = (b - a) / (range + 1);

		//For each position from start to end (both inclusive)
		float d = a;
		for (int i = 0; i <= range; i++)
		{
			//Interpolate d
			values.push_back(d);
			d += m;
		}

		return values;
	}

	//Linearly interpolate Vector2 values for each point between start and end (both inclusive)
	std::vector<Vector2> LerpRange2D(int start, int end, Vector2 a, Vector2 b)
	{
		int range = abs(start - end);
		std::vector<Vector2> values;
		values.reserve(range);

		//Slope
		Vector2 m = (b - a) / (range);

		//For each position from start to end (both inclusive)
		Vector2 d = a;
		for (int i = 0; i <= range; i++)
		{
			//Interpolate both d.x and d.y
			values.push_back(d);
			d += m;
		}

		return values;
	}
}