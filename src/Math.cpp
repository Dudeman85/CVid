#include <cvid/Math.h>
#include <cvid/Math.h>

namespace cvid
{
	//Linearly interpolate values for each point between start and end (both inclusive)
	std::vector<double> LerpRange(int start, int end, double a, double b)
	{
		int range = abs(start - end);
		std::vector<double> values;
		values.reserve(range);

		//Slope
		double m = (b - a) / (range);

		//For each position from start to end (both inclusive)
		double d = a;
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