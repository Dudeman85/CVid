#include <cvid/Rasterizer.h>

#define SWAP(a, b) {auto tmp = a; a = b; b = tmp;}

namespace cvid
{
	//Draw a line onto a window's framebuffer
	void DrawLine(Window* window, Vector2 p1, Vector2 p2, Color color)
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}
			//Make sure starting point is before ending point
			if (p1.x > p2.x)
			{
				//Swap p1 and p2
				SWAP(p1, p2)
			}

			//Keep track of closest y and the error to actual y
			int y = p1.y;
			int error = 0;

			//For each x position, plot the corresponding y
			for (int x = p1.x; x <= p2.x; x++)
			{
				window->PutPixel(x, y, color);

				error += 2 * dy;
				if (error > abs(dx))
				{
					y += yi;
					error -= 2 * dx;
				}
			}
		}
		//Slope is > 1
		else
		{
			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
			{
				//Swap p1 and p2
				SWAP(p1, p2)
			}

			//Keep track of closest x and the error to actual x
			int x = p1.x;
			int error = 0;

			//For each y position, plot the corresponding x
			for (int y = p1.y; y <= p2.y; y++)
			{
				window->PutPixel(x, y, color);

				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}
			}
		}
	}

	//Get the x coordinates of a line at every y point
	std::vector<int> InterpolateX(Vector2 p1, Vector2 p2)
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		std::vector<int> points;
		points.reserve(dy);

		//Add first point
		points.push_back(p1.x);

		//Slope is < 1
		if (abs(dx) > abs(dy))
		{
			//If slope is positive increment y, else decrement
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}
			//Make sure starting point is before ending point
			if (p1.x > p2.x)
			{
				//Swap p1 and p2
				SWAP(p1, p2)
			}

			//Keep track of closest y and the error to actual y
			int y = p1.y;
			int error = 0;

			//For each x position, plot the corresponding y
			for (int x = p1.x; x <= p2.x; x++)
			{
				error += 2 * dy;
				if (error > abs(dx))
				{
					points.push_back(x);
					y += yi;
					error -= 2 * dx;
				}
			}
		}
		//Slope is > 1
		else
		{
			//If slope is positive increment x, else decrement
			int xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}
			//Make sure starting point is before ending point
			if (p1.y > p2.y)
			{
				//Swap p1 and p2
				SWAP(p1, p2)
			}

			//Keep track of closest x and the error to actual x
			int x = p1.x;
			int error = 0;

			//For each y position, plot the corresponding x
			for (int y = p1.y; y <= p2.y; y++)
			{

				error += 2 * dx;
				if (error > abs(dy))
				{
					x += xi;
					error -= 2 * dy;
				}

				points.push_back(x);
			}
		}
		return points;
	}

	//Draw a triangle onto a window's framebuffer
	void DrawTriangle(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color)
	{
		//Sort the vertices in descending order
		if (p1.y > p2.y)
			SWAP(p1, p2)
		if (p1.y > p3.y)
			SWAP(p1, p3)
		if (p2.y > p3.y)
			SWAP(p2, p3)

		//Get every x point of each segment
		std::vector<int> combinedSegment = InterpolateX(p1, p2);
		combinedSegment.pop_back();
		std::vector<int> shortSegment = InterpolateX(p2, p3);
		combinedSegment.insert(combinedSegment.end(), shortSegment.begin(), shortSegment.end());
		std::vector<int> fullSegment = InterpolateX(p1, p3);

		//Figure out which segment is on which side
		std::vector<int>& rightSegment = combinedSegment;
		std::vector<int>& leftSegment = fullSegment;
		if (p3.x > p2.x)
		{
			leftSegment = combinedSegment;
			rightSegment = fullSegment;
		}

		//For each y coordinate in the triangle
		for (int yi = 0; yi < fullSegment.size() - 1; yi++)
		{
			//Draw a line from the full segment to the split segment
			for (int x = leftSegment[yi]; x <= rightSegment[yi]; x++)
			{
				window->PutPixel(x, yi + p1.y, color);
			}
		}
	}

	//Draw a wireframe triangle onto a window's framebuffer
	void DrawTriangleWireframe(Window* window, Vector2 p1, Vector2 p2, Vector2 p3, Color color)
	{
		DrawLine(window, p1, p2, color);
		DrawLine(window, p2, p3, color);
		DrawLine(window, p1, p3, color);
	}
}